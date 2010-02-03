/**
 * =============================================================================
 * Viper
 * Copyright (C) 2007-2010 Zach "theY4Kman" Kanzler
 * Copyright (C) 2004-2007 AlliedModders LLC.  All rights reserved.
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <Python.h>
#include "viper_metamod_wrappers.h"
#include "py_keyvalues.h"
#include <utlbuffer.h>
#include <filesystem.h>
#include <sh_tinyhash.h>
#include "viper_globals.h"
#include "python_delay_data.h"

struct KVPyObject
{
    KVPyObject() : kvpy(NULL) {}
    
    PyObject *kvpy;
};

SourceHook::THash<KeyValues *, KVPyObject> g_KeyValuesCache;

namespace SourceHook
{
    template<>
    int HashFunction<KeyValues *>(KeyValues * const &k)
    {
        return reinterpret_cast<int>(k);
    }

    template<>
    int Compare<KeyValues *>(KeyValues * const &k1, KeyValues * const &k2)
    {
        return (k1-k2);
    }
}

/* Methods */
static PyObject *
keyvalues__KeyValues__clear(keyvalues__KeyValues *self, PyObject *args,
                            PyObject *keywds)
{
    char *key = NULL;
    
    static char *keywdlist[] = {"key", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "|s", keywdlist, &key))
        return NULL;
    
    if (key != NULL)
    {
        KeyValues *findkv = self->kv->FindKey(key);
        if (findkv != NULL)
        {
            findkv->Clear();
            /* To make sure the key is now printed as a section, create a
             * hidden keyvalue */
            //findkv->CreateNewKey();
        }
    }
    else
    {
        self->kv->Clear();
        /* See above */
        //self->kv->CreateNewKey();
    }
    
    Py_RETURN_NONE;
}

static PyObject *
keyvalues__KeyValues__copy(keyvalues__KeyValues *self)
{
    return GetPyObjectFromKeyValues(self->kv->MakeCopy());
}

static PyObject *
keyvalues__KeyValues__has_key(keyvalues__KeyValues *self, PyObject *args)
{
    char *key;
    if (!PyArg_ParseTuple(args, "s", &key))
        return NULL;
    
    KeyValues *findkv = self->kv->FindKey(key);
    return PyBool_FromLong(findkv != NULL);
}

static PyObject *
keyvalues__KeyValues__parse(keyvalues__KeyValues *self, PyObject *args)
{
    char *string;
    if (!PyArg_ParseTuple(args, "s", &string))
        return NULL;
    
    return PyBool_FromLong(self->kv->LoadFromBuffer("Viper", string));
}

/* Get/Sets */
static PyObject *
keyvalues__KeyValues__nameget(keyvalues__KeyValues *self)
{
    return PyString_FromString(self->kv->GetName());
}

static int
keyvalues__KeyValues__nameset(keyvalues__KeyValues *self, PyObject *setobj)
{
    if (!PyString_Check(setobj))
    {
        PyErr_Format(_PyExc_TypeError, "KeyValues.name expects a string, not %s",
            setobj->ob_type->tp_name);
        return -1;
    }
    
    self->kv->SetName(PyString_AS_STRING(setobj));
    
    /* XXX: Not sure if setobj is a borrowed reference. Pretty sure it is. */
    //Py_DECREF(setobj);
    
    return 0;
}

static PyObject *
keyvalues__KeyValues__uses_escape_sequencesget(keyvalues__KeyValues *self)
{
    return PyBool_FromLong(self->kv->IsUsingEscapeSequences());
}

static int
keyvalues__KeyValues__uses_escape_sequencesset(keyvalues__KeyValues *self,
                                               PyObject *setobj)
{
    if (!PyBool_Check(setobj))
    {
        PyErr_Format(_PyExc_TypeError, "expected bool, found %s",
            setobj->ob_type->tp_name);
        return -1;
    }
    
    bool b = (setobj == Py_True);
    self->kv->UsesEscapeSequences(b);
    
    return 0;
}

/* Mapping Methods */
static Py_ssize_t
keyvalues__KeyValues__len__(keyvalues__KeyValues *self)
{
    KeyValues *next = self->kv->GetFirstSubKey();
    if (next == NULL)
        return 0;
    
    Py_ssize_t len = 1;
    while ((next = next->GetNextKey()) != NULL)
        len++;
    
    return len;
}

static PyObject *
keyvalues__KeyValues__subscript__(keyvalues__KeyValues *self, PyObject *key)
{
    if (!PyString_Check(key))
        return PyErr_Format(_PyExc_TypeError, "expected key of type string, not "
            "%s.", key->ob_type->tp_name);
    
    KeyValues *findkv = self->kv->FindKey(PyString_AS_STRING(key));
    
    if (findkv == NULL)
        return PyErr_Format(_PyExc_KeyError, "%s", PyString_AS_STRING(key));
    
    if (findkv->GetFirstSubKey() == NULL &&
        findkv->GetDataType() != KeyValues::TYPE_NONE)
    {
        /* A key-value pair */
        switch (findkv->GetDataType())
        {
        case KeyValues::TYPE_FLOAT:
            return PyFloat_FromDouble(findkv->GetFloat());
        
        case KeyValues::TYPE_INT:
        case KeyValues::TYPE_PTR:
            return PyInt_FromLong(findkv->GetInt());
        
        case KeyValues::TYPE_UINT64:
            return PyLong_FromUnsignedLongLong(findkv->GetUint64());
        
        case KeyValues::TYPE_COLOR:
            return CreatePyColor(findkv->GetColor());
        
        case KeyValues::TYPE_STRING:
        case KeyValues::TYPE_WSTRING:
        default:
            return PyString_FromString(findkv->GetString());
        }
    }
    else
        /* A section */
        return GetPyObjectFromKeyValues(findkv);
}

static int
keyvalues__KeyValues__ass_subscript__(keyvalues__KeyValues *self, PyObject *key,
                                      PyObject *value)
{
    if (!PyString_Check(key))
    {
        PyErr_Format(_PyExc_TypeError, "expected key of type string, not %s.",
            key->ob_type->tp_name);
        return -1;
    }
    
    if (value == NULL)
    {
        KeyValues *findkv = self->kv->FindKey(PyString_AS_STRING(key));
        if (findkv != NULL)
            self->kv->RemoveSubKey(findkv);
        
        return 0;
    }
    
    PyObject *valid_types = PyTuple_Pack(6, &keyvalues__KeyValuesType,
        _PyString_Type, _PyInt_Type, _PyFloat_Type, _PyLong_Type,
        &datatypes__ColorType);
    
    if (!PyObject_IsInstance(value, valid_types))
    {
        PyErr_Format(_PyExc_TypeError, "unexpected value type %s. Expected "
            "string, int, long, float, KeyValues, or Color.",
            value->ob_type->tp_name);
        Py_XDECREF(valid_types);
        return -1;
    }
    
    Py_XDECREF(valid_types);
    
    if (PyObject_IsInstance(value, (PyObject *)self->ob_type))
    {
        /* We have to add a new section for a KeyValues object */
        /* The key name passed here has precedence over the passed KeyValues's */
        KeyValues *kv_value = ((keyvalues__KeyValues *)value)->kv;
        assert(kv_value != NULL);
        kv_value->SetName(PyString_AS_STRING(key));
        
        KeyValues *findkv = self->kv->FindKey(PyString_AS_STRING(key));
        if (findkv != NULL)
            self->kv->RemoveSubKey(findkv);
        
        self->kv->AddSubKey(kv_value);
        return 0;
    }
    else
    {
        /* If the user just wants to add a string, we can use FindKey */
        /* Create the key if it cannot be found */
        KeyValues *findkv = self->kv->FindKey(PyString_AS_STRING(key), true);
        assert(findkv != NULL);
        
        if (findkv->GetFirstSubKey() != NULL)
        {
            /* This is a section. Remove it and create a new key. */
            self->kv->RemoveSubKey(findkv);
        }
        
        char const *strkey = PyString_AS_STRING(key);
        if (PyString_Check(value))
            self->kv->SetString(strkey, PyString_AS_STRING(value));
        else if (PyInt_Check(value))
            self->kv->SetInt(strkey, PyInt_AS_LONG(value));
        else if (PyFloat_Check(value))
            self->kv->SetFloat(strkey, PyFloat_AS_DOUBLE(value));
        else if (PyLong_Check(value))
            self->kv->SetUint64(strkey, PyLong_AsUnsignedLongLong(value));
        else if (PyObject_IsInstance(value, (PyObject *)&datatypes__ColorType))
            self->kv->SetColor(strkey, *ColorFromPyColor((datatypes__Color*)value));
        else
        {
            PyErr_SetString(g_pViperException, "DAMN YOU SAWCE! YOU HAVE "
                "FAILED ME FOR THE LAST TIME! EEEEEEEEEEEEEEEEEEEEEEP!");
            return -1;
        }
        
        return 0;
    }
}

/* Magic Methods */
static int
forwards__Forward__contains__(keyvalues__KeyValues *self, PyObject *pykey)
{
    if (!PyString_Check(pykey))
    {
        PyErr_Format(_PyExc_TypeError, "key must be a string, found type \"%s\"",
            pykey->ob_type->tp_name);
        return 0;
    }
    
    return (self->kv->FindKey(PyString_AS_STRING(pykey)) != NULL);
}

static int
keyvalues__KeyValues__cmp__(keyvalues__KeyValues *self, keyvalues__KeyValues *other)
{
    return self->kv - other->kv;
}

static void
keyvalues__KeyValues__del__(keyvalues__KeyValues *self)
{
    /* self->kv can be NULL if __init__ fails */
    if (self->kv != NULL)
    {
        RemoveKeyValuesFromCache(self->kv);
        
        self->kv->deleteThis();
        self->kv = NULL;
    }
    
    ((PyObject *)self)->ob_type->tp_free((PyObject *)self);
}

static void
RecursiveAddToKeyValuesFromDict(KeyValues *kv, PyObject *dict)
{
    PyObject *pykey, *pyvalue;
    Py_ssize_t pos = 0;
    
    while (PyDict_Next(dict, &pos, &pykey, &pyvalue))
    {
        char const *key;
        if (PyString_Check(pykey))
            key = PyString_AS_STRING(pykey);
        else
        {
            PyObject *v = PyObject_Str(pykey);
            if (v == NULL)
            {
                PyErr_Clear();
                continue;
            }
            
            key = PyString_AS_STRING(v);
            Py_DECREF(v);
        }
        
        if (PyString_Check(pyvalue))
            kv->SetString(key, PyString_AS_STRING(pyvalue));
        else if (PyInt_Check(pyvalue))
            kv->SetInt(key, PyInt_AS_LONG(pyvalue));
        else if (PyFloat_Check(pyvalue))
            kv->SetFloat(key, PyFloat_AS_DOUBLE(pyvalue));
        else if (PyLong_Check(pyvalue))
            kv->SetUint64(key, PyLong_AsUnsignedLongLong(pyvalue));
        else if (PyObject_IsInstance(pyvalue, (PyObject *)&datatypes__ColorType))
            kv->SetColor(key, *ColorFromPyColor((datatypes__Color*)pyvalue));
        else if (PyDict_Check(pyvalue))
        {
            KeyValues *subkv = new KeyValues(key);
            if (subkv == NULL)
                continue;
            
            kv->AddSubKey(subkv);
            RecursiveAddToKeyValuesFromDict(subkv, pyvalue);
        }
        else
        {
            PyObject *v = PyObject_Str(pyvalue);
            if (v == NULL)
            {
                PyErr_Clear();
                continue;
            }
            
            kv->SetString(key, PyString_AS_STRING(v));
            Py_DECREF(v);
        }
    }
}

static int
keyvalues__KeyValues__init__(keyvalues__KeyValues *self, PyObject *args,
                             PyObject *kwds)
{
    char const *name;
    PyObject *dict = NULL;
    
    static char *keywdslist[] = {"name", "dict", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s|O", keywdslist, &name, &dict))
        return -1;
    
    if (dict != NULL && !PyDict_Check(dict))
    {
        PyErr_Format(_PyExc_TypeError, "argument 2 must be dict, not %s",
            dict->ob_type->tp_name);
        return -1;
    }
    
    if (self->kv != NULL)
    {
        RemoveKeyValuesFromCache(self->kv);
        self->kv->deleteThis();
    }
    
    self->kv = new KeyValues(name);
    AddKeyValuesToCache(self->kv, (PyObject *)self);
    
    if (dict != NULL)
        RecursiveAddToKeyValuesFromDict(self->kv, dict);
    
    return 0;
}

static PyObject *
keyvalues__KeyValues__str__(keyvalues__KeyValues *self)
{
    CUtlBuffer buf(0, 0, CUtlBuffer::TEXT_BUFFER);
    self->kv->RecursiveSaveToFile(buf, 0);
    
    PyObject *str = PyString_FromStringAndSize((char const *)buf.Base(),
        buf.TellPut());
    
    return str;
}

static PyMethodDef keyvalues__KeyValues__methods[] = {
#ifdef NOT_IMPLEMENTED_YET
    {"save", (PyCFunction)keyvalues__KeyValues__save, METH_VARARGS,
        "save(file) -> bool\n\n"
        "Saves this KeyValues to a file.\n\n"
        "@type  file: str or file\n"
        "@param file: A filename to save to, or a file-like object.\n"
        "@rtype: bool\n"
        "@return: True on success, False otherwise."},
#endif
    {"clear", (PyCFunction)keyvalues__KeyValues__clear, METH_VARARGS|METH_KEYWORDS,
        "clear([key])\n\n"
        "With no arguments, removes all sub-keys. With |key|, this clears the value of\n"
        "kv[key], turning it into an empty section. If |key| is already a section, this\n"
        "is the same as kv[key].clear().\n\n"
        "@type  key: str\n"
        "@param key: The key to clear."},
    {"copy", (PyCFunction)keyvalues__KeyValues__copy, METH_NOARGS,
        "copy() -> KeyValues\n\n"
        "Recursively copies the current KeyValues into a new KeyValues.\n\n"
        "@rtype: KeyValues\n"
        "@return: A new KeyValues object with the same structure as this KeyValues."},
    {"has_key", (PyCFunction)keyvalues__KeyValues__has_key, METH_VARARGS,
        "has_key(key) -> bool\n\n"
        "Checks if the KeyValues object contains `key`. Equivalent to `key in kv`\n\n"
        "@type  key: str\n"
        "@param key: The key to check for\n"
        "@rtype: bool\n"
        "@return: True if exists, False if it doesn't."},
    {"parse", (PyCFunction)keyvalues__KeyValues__parse, METH_VARARGS,
        "parse(string)\n\n"
        "Parses the string for a KeyValues structure and loads it in.\n\n"
        "@type  string: str\n"
        "@param string: The string value to parse."},
    {NULL, NULL, 0, NULL}
};

static PyGetSetDef keyvalues__KeyValues__getsets[] = {
    {"name", (getter)keyvalues__KeyValues__nameget, (setter)keyvalues__KeyValues__nameset,
        "The root section name of this KeyValues."},
    {"uses_escape_sequences", (getter)keyvalues__KeyValues__uses_escape_sequencesget,
                              (setter)keyvalues__KeyValues__uses_escape_sequencesset,
        "Whether or not to parse escape sequences such as \\n or \\\"."},
    {NULL}
};

static PyMappingMethods keyvalues__KeyValuesMappingMethods = {
    (lenfunc)keyvalues__KeyValues__len__,                   /*mp_length*/
    (binaryfunc)keyvalues__KeyValues__subscript__,          /*mp_subscript*/
    (objobjargproc)keyvalues__KeyValues__ass_subscript__    /*mp_ass_subscript*/
};

static PySequenceMethods keyvalues__KeyValuesSequenceType = {
	0,                                                        /*sq_length*/
	0,                                                        /*sq_concat*/
	0,                                                        /*sq_repeat*/
	0,                                                        /*sq_item*/
	0,                                                        /*sq_slice*/
	0,                                                        /*sq_ass_item*/
	0,                                                        /*sq_ass_slice*/
	(objobjproc)forwards__Forward__contains__,                /*sq_contains*/
};

PyTypeObject keyvalues__KeyValuesType = {
    PyObject_HEAD_INIT(NULL)
    0,                                                      /*ob_size*/
    "sourcemod.keyvalues.KeyValues",                        /*tp_name*/
    sizeof(keyvalues__KeyValues),                           /*tp_basicsize*/
    0,                                                      /*tp_itemsize*/
    (destructor)keyvalues__KeyValues__del__,                /*tp_dealloc*/
    0,                                                      /*tp_print*/
    0,                                                      /*tp_getattr*/
    0,                                                      /*tp_setattr*/
    (cmpfunc)keyvalues__KeyValues__cmp__,                   /*tp_compare*/
    0,                                                      /*tp_repr*/
    0,                                                      /*tp_as_number*/
    &keyvalues__KeyValuesSequenceType,                      /*tp_as_sequence*/
    &keyvalues__KeyValuesMappingMethods,                    /*tp_as_mapping*/
    0,                                                      /*tp_hash */
    0,                                                      /*tp_call*/
    (reprfunc)keyvalues__KeyValues__str__,                  /*tp_str*/
    0,                                                      /*tp_getattro*/
    0,                                                      /*tp_setattro*/
    0,                                                      /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE,                 /*tp_flags*/
    "Abstracts the Valve KeyValues class.",                 /* tp_doc */
    0,                                                      /* tp_traverse */
    0,                                                      /* tp_clear */
    0,                                                      /* tp_richcompare */
    0,                                                      /* tp_weaklistoffset */
    0,//(getiterfunc)keyvalues__KeyValues__iter__,              /* tp_iter */
    0,//(iternextfunc)keyvalues__KeyValues__iternext__,         /* tp_iternext */
    keyvalues__KeyValues__methods,                          /* tp_methods */
    0,                                                      /* tp_members */
    keyvalues__KeyValues__getsets,                          /* tp_getset */
    0,                                                      /* tp_base */
    0,                                                      /* tp_dict */
    0,                                                      /* tp_descr_get */
    0,                                                      /* tp_descr_set */
    0,                                                      /* tp_dictoffset */
    (initproc)keyvalues__KeyValues__init__,                 /* tp_init */
    0,                                                      /* tp_alloc */
    &PyType_GenericNew,                                     /* tp_new */
};


/* Methods */
static PyObject *
keyvalues__keyvalues_from_file(PyObject *self, PyObject *args, PyObject *kwds)
{
    PyObject *file;
    bool use_escape_sequences = false;
    
    static char *keywdlist[] = {"file", "use_escape_sequences", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O|b", keywdlist, &file,
        &use_escape_sequences))
        return NULL;
    
    PyObject *read = NULL;
    if (!PyString_Check(file) && !(read = PyObject_GetAttrString(file, "read")))
        return PyErr_Format(_PyExc_TypeError, "expected string or file-like "
            "object, found %s", file->ob_type->tp_name);
    
    if (read != NULL)
    {
        if (!PyCallable_Check(read))
        {
            PyErr_SetString(_PyExc_TypeError, "the read function of file is not "
                "callable");
            return NULL;
        }
        
        PyObject *result = PyObject_CallObject(read, NULL);
        if (result == NULL)
            return NULL;
        
        if (!PyString_Check(result))
            return PyErr_Format(_PyExc_TypeError, "read() returned object of "
                "type %s, expected string", result->ob_type->tp_name);
        
        KeyValues *kv = new KeyValues("");
        kv->UsesEscapeSequences(use_escape_sequences);
        if (!kv->LoadFromBuffer("Viper", PyString_AS_STRING(result)))
        {
            kv->deleteThis();
            
            PyObject *pystr = PyObject_Str(file);
            char *str = PyString_AsString(pystr);
            Py_DECREF(pystr);
            
            return PyErr_Format(g_pViperException, "error loading from %s", str);
        }
        
        PyObject *kvpy = GetPyObjectFromKeyValues(kv);
        
        return kvpy;
    }
    else
    {
        KeyValues *kv = new KeyValues("");
        kv->UsesEscapeSequences(use_escape_sequences);
        if (!kv->LoadFromFile((IBaseFileSystem *)g_SMAPI->GetFileSystemFactory()
            (BASEFILESYSTEM_INTERFACE_VERSION, NULL), PyString_AS_STRING(file), NULL))
        {
            kv->deleteThis();
            return PyErr_Format(g_pViperException, "error loading from \"%s\"", 
                PyString_AS_STRING(file));
        }
        
        PyObject *kvpy = GetPyObjectFromKeyValues(kv);
        
        return kvpy;
    }
}

static PyMethodDef keyvalues__methods[] = {
    {"keyvalues_from_file", (PyCFunction)keyvalues__keyvalues_from_file, METH_VARARGS|METH_KEYWORDS,
        "keyvalues_from_file(file[, use_escape_sequences=False]) -> keyvalues.KeyValues\n\n"
        "Creates a new KeyValues object from a file or file-like object.\n\n"
        "@type  file: str or file\n"
        "@param file: If a string, creates a new KeyValues from the filename pointed to\n"
        "   by `file`. Otherwise, `file` must be a file-like object -- it must have a\n"
        "   read() function.\n"
        "@type  use_escape_sequences: bool\n"
        "@param use_escape_sequences: Whether or not to parse escape sequences such as\n"
        "   \\n or \\\"."
        "@rtype: keyvalues.KeyValues\n"
        "@return: A valid KeyValues object on success, None otherwise."},
    {NULL, NULL, 0, NULL}
};

PyObject *
initkeyvalues(void)
{
    Py_INCREF(_PyType_Type);
    keyvalues__KeyValuesType.ob_type = _PyType_Type;
    if (PyType_Ready(&keyvalues__KeyValuesType) < 0)
        return NULL;
    
    PyObject *keyvalues = Py_InitModule3("keyvalues", keyvalues__methods,
        "Abstracts the Valve KeyValues interface.");
    
    Py_INCREF((PyObject *)&keyvalues__KeyValuesType);
    PyModule_AddObject(keyvalues, "KeyValues", (PyObject *)&keyvalues__KeyValuesType);
    
    return keyvalues;
}

/* Always returns a new reference */
PyObject *
GetPyObjectFromKeyValues(KeyValues *kv)
{
    KVPyObject &obj = g_KeyValuesCache[kv];
    
    if (obj.kvpy == NULL)
    {
        PyObject *newpykv = keyvalues__KeyValuesType.tp_new(
            &keyvalues__KeyValuesType, NULL, NULL);
        ((keyvalues__KeyValues *)newpykv)->kv = kv;
        
        Py_INCREF(newpykv);
        obj.kvpy = newpykv;
        
        return newpykv;
    }
    else
    {
        Py_INCREF(obj.kvpy);
        return obj.kvpy;
    }
}

void
AddKeyValuesToCache(KeyValues *kv, PyObject *kvpy)
{
    if (kv == NULL)
        return;
    
    KVPyObject &obj = g_KeyValuesCache[kv];
    
    Py_INCREF(kvpy);
    if (obj.kvpy == NULL)
        obj.kvpy = kvpy;
    else
    {
        Py_DECREF(obj.kvpy);
        obj.kvpy = kvpy;
    }
}

void
RemoveKeyValuesFromCache(KeyValues *kv)
{
    if (kv == NULL)
        return;
    
    KVPyObject &obj = g_KeyValuesCache[kv];
    
    if (obj.kvpy != NULL)
    {
        Py_DECREF(obj.kvpy);
        obj.kvpy = NULL;
    }
}

