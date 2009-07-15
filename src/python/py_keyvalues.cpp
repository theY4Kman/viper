/**
 * =============================================================================
 * Viper
 * Copyright (C) 2008-2009 Zach "theY4Kman" Kanzler
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
#include <KeyValues.h>
#include <utlbuffer.h>
#include "py_keyvalues.h"
#include <sh_tinyhash.h>
#include "viper_globals.h"

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
    {
        return NULL;
    }
    
    if (key != NULL)
    {
        KeyValues *findkv = self->kv->FindKey(key);
        if (findkv != NULL)
        {
            findkv->Clear();
            /* To make sure the key is now printed as a section, create a
             * hidden keyvalue */
             findkv->CreateNewKey();
        }
    }
    else
    {
        self->kv->Clear();
        /* See above */
        self->kv->CreateNewKey();
    }
    
    Py_RETURN_NONE;
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
        PyErr_Format(PyExc_TypeError, "KeyValues.name expects a string, not %s",
            setobj->ob_type->tp_name);
        return -1;
    }
    
    self->kv->SetName(PyString_AS_STRING(setobj));
    
    /* XXX: Not sure if setobj is a borrowed reference. Pretty sure it is. */
    //Py_DECREF(setobj);
    
    return 0;
}

/* Mapping Methods */
static Py_ssize_t
keyvalues__KeyValues__len__(keyvalues__KeyValues *self)
{
    /* TODO: Loop over all first level keys */
    return 0;
}

static PyObject *
keyvalues__KeyValues__subscript__(keyvalues__KeyValues *self, PyObject *key)
{
    if (!PyString_Check(key))
        return PyErr_Format(PyExc_TypeError, "expected key of type string, not "
            "%s.", key->ob_type->tp_name);
    
    KeyValues *findkv = self->kv->FindKey(PyString_AS_STRING(key));
    
    if (findkv == NULL)
        Py_RETURN_NONE;
    
    /* TODO: Colors */
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
        PyErr_Format(PyExc_TypeError, "expected key of type string, not %s.",
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
    
    /* TODO: Colors */
    PyObject *valid_types = PyTuple_Pack(5, &keyvalues__KeyValuesType,
        &PyString_Type, &PyInt_Type, &PyFloat_Type, &PyLong_Type);
    
    if (!PyObject_IsInstance(value, valid_types))
    {
        PyErr_Format(PyExc_TypeError, "unexpected value type %s. Expected "
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
        
        /* TODO: Colors */
        char const *strkey = PyString_AS_STRING(key);
        if (PyString_Check(value))
            self->kv->SetString(strkey, PyString_AS_STRING(value));
        else if (PyInt_Check(value))
            self->kv->SetInt(strkey, PyInt_AS_LONG(value));
        else if (PyFloat_Check(value))
            self->kv->SetFloat(strkey, PyFloat_AS_DOUBLE(value));
        else if (PyLong_Check(value))
            self->kv->SetUint64(strkey, PyLong_AsUnsignedLongLong(value));
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
static void
keyvalues__KeyValues__del__(keyvalues__KeyValues *self)
{
    /* self->kv should be a valid KeyValues */
    assert(self->kv != NULL);
    
    RemoveKeyValuesFromCache(self->kv);
    
    self->kv->deleteThis();
    self->kv = NULL;
    
    ((PyObject *)self)->ob_type->tp_free((PyObject *)self);
}

static int
keyvalues__KeyValues__init__(keyvalues__KeyValues *self, PyObject *args,
                             PyObject *kwds)
{
    char const *name;
    static char *keywdslist[] = {"name", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s", keywdslist, &name))
        return -1;
    
    if (self->kv != NULL)
    {
        RemoveKeyValuesFromCache(self->kv);
        self->kv->deleteThis();
    }
    
    self->kv = new KeyValues(name);
    AddKeyValuesToCache(self->kv, (PyObject *)self);
    
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
    {"copy", (PyCFunction)keyvalues__KeyValues__copy, METH_NOARGS,
        "copy() -> KeyValues\n\n"
        "Recursively copies the current KeyValues into a new KeyValues.\n\n"
        "@rtype: KeyValues\n"
        "@return: A new KeyValues object with the same structure as this KeyValues."},
    {"parse", (PyCFunction)keyvalues__KeyValues__parse, METH_VARARGS,
        "parse(string)\n\n"
        "Parses the string for a KeyValues structure and loads it in.\n\n"
        "@type  string: str\n"
        "@param string: The string value to parse."},
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
    {NULL, NULL, 0, NULL}
};

static PyGetSetDef keyvalues__KeyValues__getsets[] = {
    {"name", (getter)keyvalues__KeyValues__nameget, (setter)keyvalues__KeyValues__nameset,
        "The root section name of this KeyValues."},
    {NULL}
};

static PyMappingMethods keyvalues__KeyValuesMappingMethods = {
    (lenfunc)keyvalues__KeyValues__len__,                   /*mp_length*/
    (binaryfunc)keyvalues__KeyValues__subscript__,          /*mp_subscript*/
    (objobjargproc)keyvalues__KeyValues__ass_subscript__    /*mp_ass_subscript*/
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
    0,//(cmpfunc)keyvalues__KeyValues__cmp__,                   /*tp_compare*/
    0,                                                      /*tp_repr*/
    0,                                                      /*tp_as_number*/
    0,                                                      /*tp_as_sequence*/
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

static PyMethodDef keyvalues__methods[] = {
#if NOT_IMPLEMENTED_YET
    {"keyvalues_from_file", keyvalues__keyvalues_from_file, METH_VARARGS,
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
#endif
    {NULL, NULL, 0, NULL}
};

PyObject *
initkeyvalues(void)
{
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
    KVPyObject &obj = g_KeyValuesCache[kv];
    
    if (obj.kvpy != NULL)
    {
        Py_DECREF(obj.kvpy);
        obj.kvpy = NULL;
    }
}

