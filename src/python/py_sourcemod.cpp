/**
 * =============================================================================
 * Viper
 * Copyright (C) 2010 Brandon "monokrome" Stoner
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

/**
 * @file python/py_sourcemod.cpp
 * @brief Initializes the main Viper module: sourcemod
 */

//#include <Python.h>
#include "viper_globals.h"
#include <structmember.h>
#include "python/init.h"
#include "viper.h"

static PyObject *
sourcemod__get_game_path(PyObject *self, PyObject *args)
{
    static PyObject *gamePath = PyString_FromString(g_pSM->GetGamePath());
    Py_INCREF(gamePath);
    
    return gamePath;
}

static PyObject *
sourcemod__get_sourcemod_path(PyObject *self, PyObject *args, PyObject *keywds)
{
    char *path = NULL;
    static char *kwdlist[] = {"path", NULL};
    
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "|s", kwdlist, &path))
        return NULL;

    const char *smpath = g_pSM->GetSourceModPath();
    
    if (path == NULL)
        return PyString_FromString(smpath);
    else
        return PyString_FromFormat("%s%s", smpath, path);
}

static PyMethodDef sourcemod__methods[] = {
    {"get_game_path", sourcemod__get_game_path, METH_NOARGS,
        "get_game_path() -> str\n\n"
        "Returns the full path to the game directory.\n\n"
        "@rtype: string\n"
        "@return: Returns the path to the game directory"},
    {"get_sourcemod_path", (PyCFunction)sourcemod__get_sourcemod_path, METH_VARARGS|METH_KEYWORDS,
        "get_sourcemod_path([path=\"\"]) -> str\n\n"
        "Returns the full path to SourceMod.\n\n"
        "@type  path: str\n"
        "@param path: A path to append to the SourceMod path\n"
        "@rtype: string\n"
        "@return: Returns the path to SourceMod"},
    {NULL, NULL, 0, NULL},
};

/* A class that redirects stdout to the srcds server console */
typedef struct 
{
    PyObject_HEAD
    
    PyObject *real_stderr;
    int softspace;
} sourcemod__server_err;

static PyObject *
sourcemod__server_err__write(sourcemod__server_err *self, PyObject *args)
{
    char *arg;
    if (!PyArg_ParseTuple(args, "s", &arg))
        return NULL;
    
    g_SMAPI->ConPrint(arg);
    
    // Prevent duplicate newlines at end of error log messages
    unsigned int len = strlen(arg);
    if (arg[len-1] == '\n')
        arg[len-1] = '\0';
    
    g_pSM->LogError(myself, "%s", arg);
    
    Py_RETURN_NONE;
}

static void
sourcemod__server_err__del__(sourcemod__server_err *self)
{
    PySys_SetObject("stderr", self->real_stderr);
    Py_DECREF(self->real_stderr);
    
    self->ob_type->tp_free((PyObject *)self);
}

static PyMemberDef sourcemod__server_err__members[] = {
    {"softspace", T_INT, offsetof(sourcemod__server_err, softspace), 0,
        "Flag indicating that a space needs to be printed; used by print."},
    {NULL}
};

static PyMethodDef sourcemod__server_err__methods[] = {
    {"write", (PyCFunction)sourcemod__server_err__write, METH_VARARGS,
        "write(msg)\n\n"
        "Prints an error message to the server console.\n\n"
        "@type  msg: string\n"
        "@param msg: The message to print."},
    {NULL, NULL, 0, NULL},
};

PyTypeObject sourcemod__server_errType = {
    PyObject_HEAD_INIT(NULL)
    0,                          /*ob_size*/
    "sourcemod.server_err",     /*tp_name*/
    sizeof(sourcemod__server_err),/*tp_basicsize*/
    0,                          /*tp_itemsize*/
    (destructor)sourcemod__server_err__del__,/*tp_dealloc*/
    0,                          /*tp_print*/
    0,                          /*tp_getattr*/
    0,                          /*tp_setattr*/
    0,                          /*tp_compare*/
    0,                          /*tp_repr*/
    0,                          /*tp_as_number*/
    0,                          /*tp_as_sequence*/
    0,                          /*tp_as_mapping*/
    0,                          /*tp_hash */
    0,                          /*tp_call*/
    0,                          /*tp_str*/
    PyObject_GenericGetAttr,    /*tp_getattro*/
    PyObject_GenericSetAttr,    /*tp_setattro*/
    0,                          /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT,         /*tp_flags*/
    /* tp_doc */
    "Redirects stderr to the server console.",
    0,                            /* tp_traverse */
    0,                            /* tp_clear */
    0,                            /* tp_richcompare */
    0,                            /* tp_weaklistoffset */
    0,                            /* tp_iter */
    0,                            /* tp_iternext */
    sourcemod__server_err__methods,/* tp_methods */
    sourcemod__server_err__members,/* tp_members */
    0,                          /* tp_getset */
    0,                          /* tp_base */
    0,                          /* tp_dict */
    0,                          /* tp_descr_get */
    0,                          /* tp_descr_set */
    0,                          /* tp_dictoffset */
};

/* A class that redirects stdout to the srcds server console */
typedef struct 
{
    PyObject_HEAD
    
    PyObject *real_stdout;
    int softspace;
} sourcemod__server_out;

static PyObject *
sourcemod__server_out__write(sourcemod__server_out *self, PyObject *args)
{
    char *arg;
    if (!PyArg_ParseTuple(args, "s", &arg))
        return NULL;
    
    g_SMAPI->ConPrint(arg);
    
    Py_RETURN_NONE;
}

static void
sourcemod__server_out__del__(sourcemod__server_out *self)
{
    PySys_SetObject("stdout", self->real_stdout);
    Py_DECREF(self->real_stdout);
    
    self->ob_type->tp_free((PyObject *)self);
}

static PyMemberDef sourcemod__server_out__members[] = {
    {"softspace", T_INT, offsetof(sourcemod__server_out, softspace), 0,
        "Flag indicating that a space needs to be printed; used by print."},
    {NULL}
};

static PyMethodDef sourcemod__server_out__methods[] = {
    {"write", (PyCFunction)sourcemod__server_out__write, METH_VARARGS,
        "write(msg)\n\n"
        "Prints a message to the server console.\n\n"
        "@type  msg: string\n"
        "@param msg: The message to print."},
    {NULL, NULL, 0, NULL},
};

PyTypeObject sourcemod__server_outType = {
    PyObject_HEAD_INIT(NULL)
    0,                          /*ob_size*/
    "sourcemod.server_out",     /*tp_name*/
    sizeof(sourcemod__server_out),/*tp_basicsize*/
    0,                          /*tp_itemsize*/
    (destructor)sourcemod__server_out__del__,/*tp_dealloc*/
    0,                          /*tp_print*/
    0,                          /*tp_getattr*/
    0,                          /*tp_setattr*/
    0,                          /*tp_compare*/
    0,                          /*tp_repr*/
    0,                          /*tp_as_number*/
    0,                          /*tp_as_sequence*/
    0,                          /*tp_as_mapping*/
    0,                          /*tp_hash */
    0,                          /*tp_call*/
    0,                          /*tp_str*/
    PyObject_GenericGetAttr,    /*tp_getattro*/
    PyObject_GenericSetAttr,    /*tp_setattro*/
    0,                          /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT,         /*tp_flags*/
    /* tp_doc */
    "Redirects stdout to the server console.",
    0,                            /* tp_traverse */
    0,                            /* tp_clear */
    0,                            /* tp_richcompare */
    0,                            /* tp_weaklistoffset */
    0,                            /* tp_iter */
    0,                            /* tp_iternext */
    sourcemod__server_out__methods,/* tp_methods */
    sourcemod__server_out__members,/* tp_members */
    0,                          /* tp_getset */
    0,                          /* tp_base */
    0,                          /* tp_dict */
    0,                          /* tp_descr_get */
    0,                          /* tp_descr_set */
    0,                          /* tp_dictoffset */
};

PyObject *g_pViperException = NULL;

PyObject *
initsourcemod(void)
{
    PyObject *sourcemod = Py_InitModule3("sourcemod", sourcemod__methods,
        "The standard Viper library.");
    
    if (g_pViperException == NULL)
        g_pViperException = PyErr_NewException("sourcemod.ViperError",
            NULL, NULL);
    
    Py_INCREF(g_pViperException);
    PyModule_AddObject(sourcemod, "ViperError", g_pViperException);

#define PyModule_AddStringMacroFromChar(name, string) { \
    PyObject *_name = PyString_FromString((string)); \
    Py_INCREF(_name); \
    PyModule_AddObject(sourcemod, #name, _name); }
    
    PyModule_AddStringMacroFromChar(__author__, SMEXT_CONF_AUTHOR);
    PyModule_AddStringMacroFromChar(__date__, SMEXT_CONF_DATESTRING);
    PyModule_AddStringMacroFromChar(__version__, SMEXT_CONF_VERSION);
    
    PyModule_AddIntConstant(sourcemod, "Plugin_Continue", Pl_Continue);
    PyModule_AddIntConstant(sourcemod, "Plugin_Stop", Pl_Stop);
    PyModule_AddIntConstant(sourcemod, "Plugin_Handled", Pl_Handled);
    
    /* Redirect stdout to the server console */
    sourcemod__server_outType.tp_new = PyType_GenericNew;
    sourcemod__server_errType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&sourcemod__server_outType) < 0 ||
        PyType_Ready(&sourcemod__server_errType) < 0)
    {
        g_pSM->LogError(myself, "stdout/err to server console redirection"
            " failed. `print` will output to stdout.");
        
        if (PyErr_Occurred())
            PyErr_Print();
    }
    else
    {
        PyObject *py_stdout = PySys_GetObject("stdout");
        PyObject *server_out = sourcemod__server_outType.tp_new(
            (PyTypeObject *)&sourcemod__server_outType, NULL, NULL);
        
        Py_INCREF(py_stdout);
        ((sourcemod__server_out *)server_out)->real_stdout = py_stdout;
        
        PyModule_AddObject(sourcemod, "stdout", py_stdout);
        Py_INCREF(server_out);
        PyModule_AddObject(sourcemod, "server_out", server_out);
        
        PySys_SetObject("stdout", server_out);
        
        // Now repeat for stderr
        PyObject *py_stderr = PySys_GetObject("stderr");
        PyObject *server_err = sourcemod__server_errType.tp_new(
            (PyTypeObject *)&sourcemod__server_errType, NULL, NULL);
        
        Py_INCREF(py_stderr);
        ((sourcemod__server_err *)server_err)->real_stderr = py_stderr;
        
        PyModule_AddObject(sourcemod, "stderr", py_stderr);
        Py_INCREF(server_err);
        PyModule_AddObject(sourcemod, "server_err", server_err);
        
        PySys_SetObject("stderr", server_err);
    }
        
#define PyModule_AddModuleMacro(name) { \
    PyObject *_mod_##name = init##name(); \
    Py_INCREF(_mod_##name); \
    PyModule_AddObject(sourcemod, #name, _mod_##name); }
    
    PyModule_AddModuleMacro(console);
    PyModule_AddModuleMacro(forwards);
    PyModule_AddModuleMacro(events);
    PyModule_AddModuleMacro(clients);
    PyModule_AddModuleMacro(entity);
    PyModule_AddModuleMacro(halflife);
    PyModule_AddModuleMacro(keyvalues);
    PyModule_AddModuleMacro(datatypes);
    PyModule_AddModuleMacro(usermessages);
    PyModule_AddModuleMacro(bitbuf);
    PyModule_AddModuleMacro(natives);
        
#undef  PyModule_AddModuleMacro
    
    return sourcemod;
}

