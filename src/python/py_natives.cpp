/**
 * =============================================================================
 * Viper
 * Copyright (C) 2007-2010 Zach "theY4Kman" Kanzler
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
#include <sp_vm_api.h>
#include "PluginSys.h"
#include <INativeInvoker.h>
#include "viper_globals.h"

using SourcePawn::IPluginRuntime;
using SourceMod::INativeInvoker;

static PyObject *
natives__invoke(PyObject *self, PyObject *args)
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
        else
        {
            PyErr_Format(_PyExc_TypeError, "Argument %d is not an acceptable"
                " type. Acceptable types are str, int, and float, but argument"
                " %d is type \"%s\"", i, i, arg->ob_type->tp_name);
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
    return PyInt_FromLong(result);
    
freeninv:
    delete ninv;
    return NULL;
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
    {NULL, NULL, 0, NULL}
};

PyObject *
initnatives(void)
{
    PyObject *natives = Py_InitModule3("natives", natives__methods,
        "Invocation of SourceMod natives.");
    
    return natives;
}
