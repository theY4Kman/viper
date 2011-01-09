/**
 * =============================================================================
 * Viper
 * Copyright (C) 2007-2011 Zach "theY4Kman" Kanzler
 * Copyright (C) 2004-2007 AlliedModders LLC.
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
#include <structmember.h>
#include <sp_vm_api.h>
#include "PluginSys.h"
#include <INativeInvoker.h>
#include "viper_globals.h"

using SourcePawn::IPluginRuntime;
using SourceMod::INativeInvoker;

extern PyTypeObject natives__RefType;
extern PyTypeObject natives__FloatRefType;
extern PyTypeObject natives__StringRefType;

enum RefType
{
    RefType_Cell = 0,
    RefType_Float,
    RefType_String
};

struct natives__Ref
{
    PyObject_HEAD
    
    cell_t local_addr;
    cell_t *phys_addr;
    Py_ssize_t size;
    RefType type;
};

IPluginRuntime *
GetOrCreateRuntime(IViperPlugin *pPlugin)
{
    IPluginRuntime *runtime;
    if (!pPlugin->GetProperty("NativeInvokeRuntime", (void **)&runtime))
    {
        runtime = ninvoke->CreateRuntime("Viper Natives", NINVOKE_DEFAULT_MEMORY);
        if (runtime == NULL)
        {
            PyErr_SetString(g_pViperException, "Unable to create SourcePawn"
                " plug-in runtime to invoke the native.");
            return NULL;
        }
        
        pPlugin->SetProperty("NativeInvokeRuntime", (void *)runtime);
    }
    
    return runtime;
}

static PyObject *
natives__Ref__valueget(natives__Ref *self)
{
    if (self->phys_addr == NULL)
        Py_RETURN_NONE;
    
    switch (self->type)
    {
    default:
    case RefType_Cell:
        return PyInt_FromLong(*self->phys_addr);
    
    case RefType_Float:
        return PyFloat_FromDouble(sp_ctof(*self->phys_addr));
    
    case RefType_String:
        return PyString_FromString((char const *)self->phys_addr);
    };
}

static int
natives__Ref__valueset(natives__Ref *self, PyObject *val)
{
    if (self->phys_addr == NULL)
        return 0;
    
    switch (self->type)
    {
    default:
    case RefType_Cell:
        {
            if (!PyInt_Check(val))
            {
                PyErr_Format(_PyExc_TypeError, "expected type int, got %s",
                    val->ob_type->tp_name);
                return -1;
            }
            
            *self->phys_addr = PyInt_AS_LONG(val);
            break;
        }
    
    case RefType_Float:
        {
            if (!PyFloat_Check(val))
            {
                PyErr_Format(_PyExc_TypeError, "expected type float, got %s",
                    val->ob_type->tp_name);
                return -1;
            }
            
            *self->phys_addr = sp_ftoc(PyFloat_AS_DOUBLE(val));
            break;
        }
    
    case RefType_String:
        {
            if (!PyString_Check(val))
            {
                PyErr_Format(_PyExc_TypeError, "expected type str, got %s",
                    val->ob_type->tp_name);
                return -1;
            }
            
            strncpy((char *)self->phys_addr, PyString_AS_STRING(val),
                self->size * sizeof(cell_t));
            break;
        }
    };
    
    return 0;
}

/* Magic Methods */

static int
natives__Ref__ass_item__(natives__Ref *self, Py_ssize_t n, PyObject *val)
{
    if (n > self->size)
    {
        PyErr_Format(_PyExc_IndexError, "%d is out of range, should be"
            " 0 <= n < %d", n, self->size);
        return NULL;
    }
    
    if (self->phys_addr == NULL)
        return 0;
    
    switch (self->type)
    {
    default:
    case RefType_Cell:
        {
            if (!PyInt_Check(val))
            {
                PyErr_Format(_PyExc_TypeError, "expected type int, got %s",
                    val->ob_type->tp_name);
                return -1;
            }
            
            self->phys_addr[n] = PyInt_AS_LONG(val);
            break;
        }
    
    case RefType_Float:
        {
            if (!PyFloat_Check(val))
            {
                PyErr_Format(_PyExc_TypeError, "expected type float, got %s",
                    val->ob_type->tp_name);
                return -1;
            }
            
            self->phys_addr[n] = sp_ftoc(PyFloat_AS_DOUBLE(val));
            break;
        }
    
    case RefType_String:
        {
            if (!PyString_Check(val))
            {
                PyErr_Format(_PyExc_TypeError, "expected type str, got %s",
                    val->ob_type->tp_name);
                return -1;
            }
            
            strncpy((char *)&self->phys_addr[n], PyString_AS_STRING(val),
                self->size * sizeof(cell_t) - n);
            break;
        }
    };
    
    return 0;
}

static PyObject *
natives__Ref__item__(natives__Ref *self, Py_ssize_t n)
{
    if (n > self->size)
    {
        PyErr_Format(_PyExc_IndexError, "%d is out of range, should be"
            " 0 <= n < %d", n, self->size);
        return NULL;
    }
    
    if (self->phys_addr == NULL)
        Py_RETURN_NONE;
    
    switch (self->type)
    {
    default:
    case RefType_Cell:
        return PyInt_FromLong(self->phys_addr[n]);
    
    case RefType_Float:
        return PyFloat_FromDouble((float)sp_ctof(self->phys_addr[n]));
    
    case RefType_String:
        return PyString_FromString((char const *)&self->phys_addr[n]);
    };
}

static Py_ssize_t
natives__Ref__len__(natives__Ref *self)
{
    return self->size;
}

static int
natives__Ref__init__(natives__Ref *self, PyObject *args, PyObject *keywds)
{
    Py_ssize_t size = 1;
    static char *keywdlist[] = {"size", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "|n", keywdlist, &size))
        return -1;
    
    GET_THREAD_PLUGIN();
    IPluginRuntime *runtime = GetOrCreateRuntime(pPlugin);
    
    cell_t cell_size = size;
    if (self->type == RefType_String)
    {
        int mod = size % 4;
        cell_size = (cell_size / 4) + (mod == 0);
    }
    
    if (runtime->GetDefaultContext()->HeapAlloc(size, &self->local_addr,
        &self->phys_addr) != SP_ERROR_NONE)
    {
        PyErr_Format(g_pViperException, "Unable to allocate %d cells!", size);
        self->size = 0;
        
        return -1;
    }
    
    self->size = size;
    
    return 0;
}

static PyObject *
natives__Ref__new__(PyTypeObject *type, PyObject *args, PyObject *keywds)
{
    natives__Ref *ref = (natives__Ref *)type->tp_alloc(type, 1);
    ref->local_addr = 0;
    ref->phys_addr = NULL;
    ref->size = 0;
    
    ref->type = RefType_Cell;
    if (type == &natives__FloatRefType)
        ref->type = RefType_Float;
    else if (type == &natives__StringRefType)
        ref->type = RefType_Float;
    
    return (PyObject *)ref;
}

static PyObject *
natives__Ref__repr__(natives__Ref *self)
{
    switch (self->type)
    {
    default:
    case RefType_Cell:
        return PyString_FromFormat("<Cell[%d] Reference at %d (%p): %d>",
            self->size, self->local_addr, self->phys_addr, *self->phys_addr);
    
    case RefType_Float:
        return PyString_FromFormat("<Float[%d] Reference at %d (%p): %f>",
            self->size, self->local_addr, self->phys_addr,
            (float)*self->phys_addr);
    
    case RefType_String:
        return PyString_FromFormat("<String[%d] Reference at %d (%p): \"%s\">",
            self->size, self->local_addr, self->phys_addr,
            (char const *)self->phys_addr);
    };
    
    return PyString_FromFormat("<Reference at %d (%p)>", self->local_addr,
        self->phys_addr);
}

static PyMemberDef natives__Ref__members[] = {
    {"size", T_UINT, offsetof(natives__Ref, size), READONLY,
        "The number of cells allocated for this reference."},
    {NULL},
};

static PyGetSetDef natives__Ref__getsets[] = {
    {"value", (getter)natives__Ref__valueget, (setter)natives__Ref__valueset,
        "The value of the reference."},
    {NULL},
};

static PyGetSetDef natives__StringRef__getsets[] = {
    {"value", (getter)natives__Ref__valueget, (setter)natives__Ref__valueset,
        "The value of the reference."},
    {NULL},
};

PySequenceMethods natives__RefSequenceMethods = {
    (lenfunc)natives__Ref__len__,                       /* sq_length */
    0,                                                  /* sq_concat */
    0,                                                  /* sq_repeat */
    (ssizeargfunc)natives__Ref__item__,                 /* sq_item */
    0,                                                  /* sq_slice */
    (ssizeobjargproc)natives__Ref__ass_item__,          /* sq_ass_item */
    0,                                                  /* sq_ass_slice */
//    (objobjarg)natives__Ref__contains__,                /* sq_contains */
};

PyTypeObject natives__RefType = {
    PyObject_HEAD_INIT(_PyType_Type)
    0,                                                      /*ob_size*/
    "sourcemod.natives.Ref",                                /*tp_name*/
    sizeof(natives__Ref),                                   /*tp_basicsize*/
    0,                                                      /*tp_itemsize*/
    0,                                                      /*tp_dealloc*/
    0,                                                      /*tp_print*/
    0,                                                      /*tp_getattr*/
    0,                                                      /*tp_setattr*/
    0,                                                      /*tp_compare*/
    (reprfunc)natives__Ref__repr__,                         /*tp_repr*/
    0,                                                      /*tp_as_number*/
    &natives__RefSequenceMethods,                           /*tp_as_sequence*/
    0,                                                      /*tp_as_mapping*/
    0,                                                      /*tp_hash */
    0,                                                      /*tp_call*/
    0,                                                      /*tp_str*/
    0,                                                      /*tp_getattro*/
    0,                                                      /*tp_setattro*/
    0,                                                      /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE,                 /*tp_flags*/
    "A reference to memory in a SourcePawn runtime",        /* tp_doc */
    0,                                                      /* tp_traverse */
    0,                                                      /* tp_clear */
    0,                                                      /* tp_richcompare */
    0,                                                      /* tp_weaklistoffset */
    0,                                                      /* tp_iter */
    0,                                                      /* tp_iternext */
    0,                                                      /* tp_methods */
    0,                                                      /* tp_members */
    natives__Ref__getsets,                                  /* tp_getset */
    0,                                                      /* tp_base */
    0,                                                      /* tp_dict */
    0,                                                      /* tp_descr_get */
    0,                                                      /* tp_descr_set */
    0,                                                      /* tp_dictoffset */
    (initproc)natives__Ref__init__,                         /* tp_init */
    0,                                                      /* tp_alloc */
    (newfunc)natives__Ref__new__,                           /* tp_new */
};

PyTypeObject natives__FloatRefType = {
    PyObject_HEAD_INIT(_PyType_Type)
    0,                                                      /*ob_size*/
    "sourcemod.natives.FloatRef",                           /*tp_name*/
    sizeof(natives__Ref),                                   /*tp_basicsize*/
    0,                                                      /*tp_itemsize*/
    0,                                                      /*tp_dealloc*/
    0,                                                      /*tp_print*/
    0,                                                      /*tp_getattr*/
    0,                                                      /*tp_setattr*/
    0,                                                      /*tp_compare*/
    (reprfunc)natives__Ref__repr__,                         /*tp_repr*/
    0,                                                      /*tp_as_number*/
    0,                                                      /*tp_as_sequence*/
    0,                                                      /*tp_as_mapping*/
    0,                                                      /*tp_hash */
    0,                                                      /*tp_call*/
    0,                                                      /*tp_str*/
    0,                                                      /*tp_getattro*/
    0,                                                      /*tp_setattro*/
    0,                                                      /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE,                 /*tp_flags*/
    "A reference to a float in a SourcePawn runtime",       /* tp_doc */
    0,                                                      /* tp_traverse */
    0,                                                      /* tp_clear */
    0,                                                      /* tp_richcompare */
    0,                                                      /* tp_weaklistoffset */
    0,                                                      /* tp_iter */
    0,                                                      /* tp_iternext */
    0,                                                      /* tp_methods */
    natives__Ref__members,                                  /* tp_members */
    natives__Ref__getsets,                                  /* tp_getset */
    &natives__RefType,                                      /* tp_base */
    0,                                                      /* tp_dict */
    0,                                                      /* tp_descr_get */
    0,                                                      /* tp_descr_set */
    0,                                                      /* tp_dictoffset */
    (initproc)natives__Ref__init__,                         /* tp_init */
    0,                                                      /* tp_alloc */
    (newfunc)natives__Ref__new__,                           /* tp_new */
};

PyTypeObject natives__StringRefType = {
    PyObject_HEAD_INIT(_PyType_Type)
    0,                                                      /*ob_size*/
    "sourcemod.natives.StringRef",                           /*tp_name*/
    sizeof(natives__Ref),                                   /*tp_basicsize*/
    0,                                                      /*tp_itemsize*/
    0,                                                      /*tp_dealloc*/
    0,                                                      /*tp_print*/
    0,                                                      /*tp_getattr*/
    0,                                                      /*tp_setattr*/
    0,                                                      /*tp_compare*/
    (reprfunc)natives__Ref__repr__,                         /*tp_repr*/
    0,                                                      /*tp_as_number*/
    0,                                                      /*tp_as_sequence*/
    0,                                                      /*tp_as_mapping*/
    0,                                                      /*tp_hash */
    0,                                                      /*tp_call*/
    0,                                                      /*tp_str*/
    0,                                                      /*tp_getattro*/
    0,                                                      /*tp_setattro*/
    0,                                                      /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE,                 /*tp_flags*/
    "A reference to a string in a SourcePawn runtime",      /* tp_doc */
    0,                                                      /* tp_traverse */
    0,                                                      /* tp_clear */
    0,                                                      /* tp_richcompare */
    0,                                                      /* tp_weaklistoffset */
    0,                                                      /* tp_iter */
    0,                                                      /* tp_iternext */
    0,                                                      /* tp_methods */
    natives__Ref__members,                                  /* tp_members */
    0,                                                      /* tp_getset */
    &natives__RefType,                                      /* tp_base */
    0,                                                      /* tp_dict */
    0,                                                      /* tp_descr_get */
    0,                                                      /* tp_descr_set */
    0,                                                      /* tp_dictoffset */
    (initproc)natives__Ref__init__,                         /* tp_init */
    0,                                                      /* tp_alloc */
    (newfunc)natives__Ref__new__,                           /* tp_new */
};

static PyObject *
NativesInvoke(PyObject *args, bool floatCast)
{
    char const *native;
    PyObject *py_native;
    
    INativeInvoker *ninv;
    IPluginRuntime *runtime;
    
    int result;
    int error;
    
    unsigned int length = PyTuple_Size(args);
    
    if (length < 1)
    {
        PyErr_SetString(_PyExc_TypeError, "Not enough arguments (need at least"
            " 1)");
        return NULL;
    }
    
    py_native = PyTuple_GetItem(args, 0);
    if (!PyString_Check(py_native))
    {
        PyErr_Format(_PyExc_TypeError, "`native` is wrong type (expected str,"
            " got %s)", py_native->ob_type->tp_name);
        return NULL;
    }
    
    native = PyString_AS_STRING(py_native);
    
    GET_THREAD_PLUGIN();
    runtime = GetOrCreateRuntime(pPlugin);
    
    ninv = ninvoke->CreateInvoker();
    if (!ninv->Start(runtime->GetDefaultContext(), native))
    {
        PyErr_Format(g_pViperException, "Native \"%s\" does not exist", native);
        goto freeninv;
    }
    
    for (unsigned int i=1; i<length; i++)
    {
        PyObject *arg = PyTuple_GetItem(args, i);
        
        if (PyString_Check(arg))
            ninv->PushString(PyString_AS_STRING(arg));
        else if (PyInt_Check(arg))
            ninv->PushCell(PyInt_AS_LONG(arg));
        else if (PyFloat_Check(arg))
            ninv->PushFloat(PyFloat_AS_DOUBLE(arg));
        else if (PyObject_IsInstance(arg, (PyObject *)&natives__RefType))
        {
            natives__Ref *ref = (natives__Ref *)arg;
            if (ref->phys_addr == NULL)
            {
                PyErr_Format(g_pViperException, "Argument %d is a null"
                    " reference", i);
                goto freeninv;
            }
            
            switch (ref->type)
            {
            default:
            case RefType_Cell:
                ninv->PushCellByRef(ref->phys_addr);
                break;
            
            case RefType_Float:
                ninv->PushFloatByRef((float *)ref->phys_addr);
                break;
            
            case RefType_String:
                ninv->PushStringEx((char *)ref->phys_addr, ref->size, 0,
                    SM_PARAM_COPYBACK);
                break;
            };
        }
        else
        {
            PyErr_Format(_PyExc_TypeError, "Argument %d is not an acceptable"
                " type. Acceptable types are str, int, float, and natives.Ref,"
                " but argument %d is type \"%s\"", i, i, arg->ob_type->tp_name);
            goto freeninv;
        }
    }
    
    error = ninv->Invoke(&result);
    if (error != SP_ERROR_NONE)
    {
        /* TODO: error string */
        PyErr_Format(g_pViperException, "Native returned error: %d", error);
        goto freeninv;
    }
    
    delete ninv;
    
    if (floatCast)
        return PyFloat_FromDouble(sp_ctof(result));
    else
        return PyInt_FromLong(result);
    
freeninv:
    delete ninv;
    return NULL;
}

static PyObject *
natives__invoke(PyObject *self, PyObject *args)
{
    return NativesInvoke(args, false);
}

static PyObject *
natives__invoke_float(PyObject *self, PyObject *args)
{
    return NativesInvoke(args, true);
}

static PyMethodDef natives__methods[] = {
    {"invoke", (PyCFunction)natives__invoke, METH_VARARGS,
        "invoke(native, ...) -> int\n\n"
        "Calls a SourceMod native.\n"
        "@type  native: str\n"
        "@param native: The name of the native to invoke.\n"
        "@rtype: int\n"
        "@return: The cell value returned by the native.\n"
        "@throw ViperError: Native does not exist.\n"
        "@throw ViperError: Native invocation failed."},
    {"invoke_float", (PyCFunction)natives__invoke_float, METH_VARARGS,
        "invoke_float(native, ...) -> float\n\n"
        "Calls a SourceMod native and casts its return value to float.\n"
        "@type  native: str\n"
        "@param native: The name of the native to invoke.\n"
        "@rtype: float\n"
        "@return: The float value returned by the native.\n"
        "@throw ViperError: Native does not exist.\n"
        "@throw ViperError: Native invocation failed."},
    {NULL, NULL, 0, NULL}
};

PyObject *
initnatives(void)
{
    if (PyType_Ready(&natives__RefType) < 0 ||
        PyType_Ready(&natives__FloatRefType) < 0)
    {
        PyErr_Print();
        return NULL;
    }
    
    PyObject *natives = Py_InitModule3("natives", natives__methods,
        "Invocation of SourceMod natives.");
    
    Py_INCREF((PyObject *)&natives__RefType);
    PyModule_AddObject(natives, "Ref", (PyObject *)&natives__RefType);
    
    Py_INCREF((PyObject *)&natives__FloatRefType);
    PyModule_AddObject(natives, "FloatRef", (PyObject *)&natives__FloatRefType);
    
    return natives;
}
