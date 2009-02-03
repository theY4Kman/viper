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
#include <structmember.h>
#include "viper_globals.h"
#include "systems/ConCmdManager.h"
#include "python/py_console.h"
#include "IViperForwardSys.h"

static PyObject *
console__ConCommandReply__reply(PyObject *self, PyObject *args)
{
    // :TODO: Reply to the user in the way they called the concmd
    char const *message = NULL;
    if (!PyArg_ParseTuple(args, "s", &message))
        return NULL;

	g_SMAPI->ConPrintf("%s\n", message);
    
    Py_RETURN_NONE;
}

static PyMethodDef console__ConCommandReply__methods[] = {
    {"reply", console__ConCommandReply__reply, METH_VARARGS,
        "Replies to the client whom executed the ConCommand in the way they\n"
        "executed the ConCommand (console or chat).\n"
        "@type  message: string\n"
        "@param message: The message to send"},
    {NULL, NULL, 0, NULL},
};

static PyMemberDef console__ConCommandReply__members[] = {
    {"args", T_OBJECT_EX, offsetof(console__ConCommandReply, args), READONLY,
        "The arguments passed when the ConCommand was executed"},
    {"name", T_STRING, offsetof(console__ConCommandReply, name), READONLY,
        "The name of the ConCommand executed."},
    {"client", T_INT, offsetof(console__ConCommandReply, client), READONLY,
        "The client whom executed the ConCommand"},
    {"argstring", T_STRING, offsetof(console__ConCommandReply, argstring), READONLY,
        "The full argument string sent to the server. This includes any quotes sent."},
    {NULL},
};

PyTypeObject console__ConCommandReplyType = {
    PyObject_HEAD_INIT(NULL)
    0,                          /*ob_size*/
    "sourcemod.console.ConCommandReply",/*tp_name*/
    sizeof(console__ConCommandReply),/*tp_basicsize*/
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
    "Stores information about a ConCommand when it is executed.",
    0,		                    /* tp_traverse */
    0,		                    /* tp_clear */
    0,		                    /* tp_richcompare */
    0,		                    /* tp_weaklistoffset */
    0,		                    /* tp_iter */
    0,		                    /* tp_iternext */
    console__ConCommandReply__methods,/* tp_methods */
    console__ConCommandReply__members,/* tp_members */
    0,                          /* tp_getset */
    0,                          /* tp_base */
    0,                          /* tp_dict */
    0,                          /* tp_descr_get */
    0,                          /* tp_descr_set */
    0,                          /* tp_dictoffset */
};

static PyObject *
console__print_to_server(PyObject *self, PyObject *args)
{
	char *message;

	if(!PyArg_ParseTuple(args, "s", &message))
		return NULL;

	g_SMAPI->ConPrintf("%s\n", message);
	
	Py_RETURN_NONE;
}

static PyObject *
console__reg_concmd(PyObject *self, PyObject *args, PyObject *keywds)
{
    char const *sName = NULL;
    char const *sDescription = "";
    int flags = 0;
    PyFunction *pFunc;
    
    static char *kwdlist[] = {"name", "callback", "description", "flags", NULL};
    
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "sO|si", kwdlist,
            &sName, &pFunc, &sDescription, &flags))
        return NULL;
    
    if (!PyCallable_Check(pFunc))
    {
        PyErr_SetString(g_pViperException, "The callback passed was not callable");
        return NULL;
    }
    
    if (!IS_STR_FILLED(sName))
    {
        PyErr_SetString(g_pViperException, "The name passed was blank");
        return NULL;
    }
    
    Py_INCREF(pFunc);
    
    PyObject *thread_dict = PyThreadState_GetDict();
    PyObject *pyPlugin = PyDict_GetItemString(thread_dict, "viper_cplugin");
    
    IViperPlugin *pPlugin;
    if (pyPlugin == NULL
        || (pPlugin = (IViperPlugin*)PyCObject_AsVoidPtr(pyPlugin)) == NULL)
    {
        PyErr_SetString(g_pViperException, "The current thread state has no "
            "plug-in associated");
        return NULL;
    }
    
    if (!g_VCmds.AddCommand(pPlugin, pFunc, Cmd_Console, sName, sDescription, flags))
        Py_RETURN_FALSE;
    
    Py_RETURN_TRUE;
}

static PyObject *
console__server_command(PyObject *self, PyObject *args)
{
	char *command;

	if(!PyArg_ParseTuple(args, "s", &command))
		return NULL;

	engine->ServerCommand(command);
	
	Py_RETURN_NONE;
}

static PyMethodDef console__methods[] = {
#if NOT_IMPLEMENTED_YET
    {"create_convar", (PyCFunction)console__create_convar, METH_VARARGS|METH_KEYWORDS,
        "Creates a new console variable.\n"
        "@type  name: string\n"
        "@param name: Name of the ConVar\n"
        "@type  value: string\n"
        "@param value: The value of the ConVar. As the internal representation\n"
        "   of a ConVar's value is a string, stringization is left to the user.\n"
        "@type  description: string\n"
        "@param description: (Optional) Description of the ConVar\n"
        "@type  flags: FCVAR constants\n"
        "@param flags: (Optional) Flags that change how a ConVar is handled.\n"
        "   Use FCVAR constants, such as FCVAR_CHEAT, etc."},
#endif
	{"print_to_server", console__print_to_server, METH_VARARGS,
	    "Sends a message to the server console.\n"
	    "@type  message: string\n"
	    "@param message: The message to print"},
    {"reg_concmd", (PyCFunction)console__reg_concmd, METH_VARARGS|METH_KEYWORDS,
        "Registers a new console command or hooks an existing one.\n"
        "@type  name: string\n"
        "@param name: Name of the ConCommand\n"
        "@type  callback: callable\n"
        "@param callback: A function to call when the ConCommand is executed.\n"
        "   The function should receive one argument: a sourcemod.console.ConCommand object.\n"
        "@type  description: string\n"
        "@param description: (Optional) Description of the ConCommand\n"
        "@type  flags: FCVAR constants\n"
        "@param flags: (Optional) Flags that change how a ConCommand is handled.\n"
        "   Use FCVAR constants, such as FCVAR_CHEAT, etc."},
	{"server_command", console__server_command, METH_VARARGS,
	    "Executes a command as if it were on the server console\n"
	    "@type  command: string\n"
	    "@param command: Command to execute"},
    {NULL, NULL, 0, NULL},
};

PyObject *
initconsole(void)
{
    if (PyType_Ready((PyTypeObject*)&console__ConCommandReplyType) < 0)
        return NULL;
    
    PyObject *console = Py_InitModule3("console", console__methods,
        "Contains functions and objects pertaining to console interaction.");
    
    PyModule_AddObject(console, "ConCommand", (PyObject*)&console__ConCommandReplyType);
    
    PyModule_AddIntConstant(console, "Plugin_Continue", Pl_Continue);
    PyModule_AddIntConstant(console, "Plugin_Stop", Pl_Stop);
    PyModule_AddIntConstant(console, "Plugin_Handled", Pl_Handled);
    
    PyModule_AddIntMacro(console, FCVAR_NONE);
    PyModule_AddIntMacro(console, FCVAR_UNREGISTERED);
    PyModule_AddIntMacro(console, FCVAR_GAMEDLL);
    PyModule_AddIntMacro(console, FCVAR_CLIENTDLL);
    PyModule_AddIntMacro(console, FCVAR_PROTECTED);
    PyModule_AddIntMacro(console, FCVAR_SPONLY);
    PyModule_AddIntMacro(console, FCVAR_ARCHIVE);
    PyModule_AddIntMacro(console, FCVAR_NOTIFY);
    PyModule_AddIntMacro(console, FCVAR_USERINFO);
    PyModule_AddIntMacro(console, FCVAR_PRINTABLEONLY);
    PyModule_AddIntMacro(console, FCVAR_UNLOGGED);
    PyModule_AddIntMacro(console, FCVAR_NEVER_AS_STRING);
    PyModule_AddIntMacro(console, FCVAR_REPLICATED);
    PyModule_AddIntMacro(console, FCVAR_CHEAT);
    PyModule_AddIntMacro(console, FCVAR_DEMO);
    PyModule_AddIntMacro(console, FCVAR_DONTRECORD);
    PyModule_AddIntMacro(console, FCVAR_NOT_CONNECTED);
    PyModule_AddIntMacro(console, FCVAR_ARCHIVE_XBOX);
    
    /* Manually define the next ones, as they are not defined in OB */
    PyModule_AddIntConstant(console, "FCVAR_LAUNCHER",          (1<<1));
    PyModule_AddIntConstant(console, "FCVAR_MATERIAL_SYSTEM",   (1<<4));
    PyModule_AddIntConstant(console, "FCVAR_STUDIORENDER",      (1<<15));
    PyModule_AddIntConstant(console, "FCVAR_PLUGIN",            (1<<18));
    PyModule_AddIntConstant(console, "FCVAR_DATACACHE",         (1<<19));
    PyModule_AddIntConstant(console, "FCVAR_TOOLSYSTEM",        (1<<20));
    PyModule_AddIntConstant(console, "FCVAR_FILESYSTEM",        (1<<21));
    PyModule_AddIntConstant(console, "FCVAR_SOUNDSYSTEM",       (1<<23));
    PyModule_AddIntConstant(console, "FCVAR_INPUTSYSTEM",       (1<<25));
    PyModule_AddIntConstant(console, "FCVAR_NETWORKSYSTEM",     (1<<26));
    PyModule_AddIntConstant(console, "FCVAR_VPHYSICS",          (1<<27));
    
    return console;
}
