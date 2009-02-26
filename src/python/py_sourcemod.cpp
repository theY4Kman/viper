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

/**
 * @file python/py_sourcemod.cpp
 * @brief Initializes the main Viper module: sourcemod
 */

#include <Python.h>
#include "viper_globals.h"
#include "python/init.h"

static PyObject *
sourcemod__get_game_path(PyObject *self, PyObject *args)
{
	PyObject *gamePath = PyString_FromString(g_pSM->GetGamePath());
	Py_INCREF(gamePath);
	
	return gamePath;
}

static PyObject *
sourcemod__get_sourcemod_path(PyObject *self, PyObject *args)
{
	PyObject *smPath = PyString_FromString(g_pSM->GetSourceModPath());
	Py_INCREF(smPath);
	
	return smPath;
}

static PyMethodDef sourcemod__methods[] = {
    {"get_game_path", sourcemod__get_game_path, METH_NOARGS,
        "get_game_path() -> str\n\n"
        "Returns the full path to the game directory.\n"
        "@rtype: string\n"
        "@return: Returns the path to the game directory"},
    {"get_sourcemod_path", sourcemod__get_sourcemod_path, METH_NOARGS,
        "get_sourcemod_path() -> str\n\n"
        "Returns the full path to SourceMod.\n"
        "@rtype: string\n"
        "@return: Returns the path to SourceMod"},
    {NULL, NULL, 0, NULL},
};

/* A class that redirects stdout to the srcds server console */
typedef struct 
{
    PyObject_HEAD
} sourcemod__server_out;

static PyObject *
sourcemod__server_out__write(PyObject *self, PyObject *args)
{
    char *arg;
    if (!PyArg_ParseTuple(args, "s", &arg))
        return NULL;
    
    /* This is a total hack! Unless the argument passed is a newline, we append
     * a space to the message, so that `print >> stdout, "hi", "there"` looks
     * the same as `print "hi", "there"`
     */
    if (*arg != '\n')
        g_SMAPI->ConPrintf("%s ", arg);
    else
        g_SMAPI->ConPrint("\n");
    
    Py_RETURN_NONE;
}

static PyMethodDef sourcemod__server_out__methods[] = {
    {"write", sourcemod__server_out__write, METH_STATIC|METH_VARARGS,
        "write(msg)\n\n"
        "Prints a message to the server console.\n"
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
    0,                          /*tp_dealloc*/
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
    0,                          /*tp_getattro*/
    0,                          /*tp_setattro*/
    0,                          /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT,         /*tp_flags*/
    /* tp_doc */
    "Redirects stdout to the server console.",
    0,		                    /* tp_traverse */
    0,		                    /* tp_clear */
    0,		                    /* tp_richcompare */
    0,		                    /* tp_weaklistoffset */
    0,		                    /* tp_iter */
    0,		                    /* tp_iternext */
    sourcemod__server_out__methods,/* tp_methods */
    0,                          /* tp_members */
    0,                          /* tp_getset */
    0,                          /* tp_base */
    0,                          /* tp_dict */
    0,                          /* tp_descr_get */
    0,                          /* tp_descr_set */
    0,                          /* tp_dictoffset */
};

PyObject *g_pViperException = NULL;

void
initsourcemod(void)
{
    PyObject *sourcemod = Py_InitModule3("sourcemod", sourcemod__methods,
        "The standard Viper library.");
    
    g_pViperException = PyErr_NewException("sourcemod.ViperError", NULL, NULL);
    Py_INCREF(g_pViperException);
    PyModule_AddObject(sourcemod, "ViperError", g_pViperException);
    
#define PyModule_AddModuleMacro(name) {\
    PyObject *_name = init##name();\
    Py_XINCREF(_name);\
    PyModule_AddObject(sourcemod, #name, _name); }
    
    PyModule_AddModuleMacro(console);
    PyModule_AddModuleMacro(forwards);
    PyModule_AddModuleMacro(events);
    
    /* Redirect stdout to the server console */
    if (PyType_Ready((PyTypeObject*)&sourcemod__server_outType) < 0)
        g_pSM->LogError(myself, "stdout to server console redirection failed."
            " `print` will output to stdout.");
    else
    {
        sourcemod__server_outType.tp_new = PyType_GenericNew;
        
        PyObject *sys = PyImport_AddModule("sys");
        PyObject *py_stdout = PyObject_GetAttrString(sys, "stdout");
        
        PyObject *server_out = sourcemod__server_outType.tp_new(&sourcemod__server_outType, NULL, NULL);
        
        PyModule_AddObject(sourcemod, "stdout", py_stdout);
        PyModule_AddObject(sourcemod, "server_out", server_out);
        
        PyObject_SetAttrString(sys, "stdout", server_out);
    }
}

