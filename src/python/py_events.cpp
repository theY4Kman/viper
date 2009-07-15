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

#include "py_events.h"
#include "EventManager.h"
#include "PluginSys.h"
#include <structmember.h>
#include <bitbuf.h>

/***** TODO: destroy Event object when its IGameEvent is freed ******/

static PyObject *
events__Event__cancel(events__Event *self)
{
    if (self->event == NULL)
    {
        PyErr_SetString(g_pViperException, "Invalid game event.");
        return NULL;
    }
    
    gameevents->FreeEvent(self->event);
    
    /* Invalidate this object */
    self->event = NULL;
    
    Py_RETURN_NONE;
}

static PyObject *
events__Event__fire(events__Event *self, PyObject *args)
{
    if (self->event == NULL)
    {
        PyErr_SetString(g_pViperException, "Invalid game event.");
        return NULL;
    }
    
    bool dont_broadcast = false;
    if (!PyArg_ParseTuple(args, "b", &dont_broadcast))
        return NULL;
    
    gameevents->FireEvent(self->event, dont_broadcast);
    
    /* Invalidate this object */
    self->event = NULL;
    
    Py_RETURN_NONE;
}

static PyObject *
events__Event__get_bool(events__Event *self, PyObject *args)
{
    if (self->event == NULL)
    {
        PyErr_SetString(g_pViperException, "Invalid game event.");
        return NULL;
    }
    
    char const *field;
    if (!PyArg_ParseTuple(args, "s", &field))
        return NULL;
    
    return Py_BuildValue("b", self->event->GetBool(field));
}

static PyObject *
events__Event__get_float(events__Event *self, PyObject *args)
{
    if (self->event == NULL)
    {
        PyErr_SetString(g_pViperException, "Invalid game event.");
        return NULL;
    }
    
    char const *field;
    if (!PyArg_ParseTuple(args, "s", &field))
        return NULL;
    
    return Py_BuildValue("f", self->event->GetFloat(field));
}

static PyObject *
events__Event__get_int(events__Event *self, PyObject *args)
{
    if (self->event == NULL)
    {
        PyErr_SetString(g_pViperException, "Invalid game event.");
        return NULL;
    }
    
    char const *field;
    if (!PyArg_ParseTuple(args, "s", &field))
        return NULL;
    
    return Py_BuildValue("i", self->event->GetInt(field));
}

static PyObject *
events__Event__get_string(events__Event *self, PyObject *args)
{
    if (self->event == NULL)
    {
        PyErr_SetString(g_pViperException, "Invalid game event.");
        return NULL;
    }
    
    char const *field;
    if (!PyArg_ParseTuple(args, "s", &field))
        return NULL;
    
    return PyString_FromString(self->event->GetString(field));
}

static PyObject *
events__Event__has_field(events__Event *self, PyObject *args)
{
    if (self->event == NULL)
    {
        PyErr_SetString(g_pViperException, "Invalid game event.");
        return NULL;
    }
    
    char const *field;
    if (!PyArg_ParseTuple(args, "s", &field))
        return NULL;
    
    return Py_BuildValue("b", !self->event->IsEmpty(field));
}

static PyObject *
events__Event__set_bool(events__Event *self, PyObject *args)
{
    if (self->event == NULL)
    {
        PyErr_SetString(g_pViperException, "Invalid game event.");
        return NULL;
    }
    
    char const *field;
    bool value;
    
    if (!PyArg_ParseTuple(args, "sb", &field, &value))
        return NULL;
    
    self->event->SetBool(field, value);
    
    Py_RETURN_NONE;
}

static PyObject *
events__Event__set_float(events__Event *self, PyObject *args)
{
    if (self->event == NULL)
    {
        PyErr_SetString(g_pViperException, "Invalid game event.");
        return NULL;
    }
    
    char const *field;
    float value;
    
    if (!PyArg_ParseTuple(args, "sf", &field, &value))
        return NULL;
    
    self->event->SetFloat(field, value);
    
    Py_RETURN_NONE;
}

static PyObject *
events__Event__set_int(events__Event *self, PyObject *args)
{
    if (self->event == NULL)
    {
        PyErr_SetString(g_pViperException, "Invalid game event.");
        return NULL;
    }
    
    char const *field;
    int value;
    
    if (!PyArg_ParseTuple(args, "si", &field, &value))
        return NULL;
    
    self->event->SetInt(field, value);
    
    Py_RETURN_NONE;
}

static PyObject *
events__Event__set_string(events__Event *self, PyObject *args)
{
    if (self->event == NULL)
    {
        PyErr_SetString(g_pViperException, "Invalid game event.");
        return NULL;
    }
    
    char const *field;
    char const *value;
    
    if (!PyArg_ParseTuple(args, "ss", &field, &value))
        return NULL;
    
    self->event->SetString(field, value);
    
    Py_RETURN_NONE;
}

static PyObject *
events__Event__str__(events__Event *self)
{
    if (self->event == NULL)
        return PyString_FromString("<Invalid Event>");
    
    return PyString_FromFormat("<Event \"%s\">", self->event->GetName());
}

static PyObject *
events__Event__nameget(events__Event *self)
{
    if (self->event == NULL)
    {
        PyErr_SetString(g_pViperException, "Invalid game event.");
        return NULL;
    }
    
    return PyString_FromString(self->event->GetName());
}

static PyMemberDef events__Event__members[] = {
    {"dont_broadcast", T_UBYTE, offsetof(events__Event, bDontBroadcast), READONLY,
        "Whether or not this event will be broadcast to players."},
    {NULL},
};

static PyGetSetDef events__Event__getsets[] = {
    {"name", (getter)events__Event__nameget, NULL,
        "The name of this game event.", NULL},
    {NULL},
};

static PyMethodDef events__Event__methods[] = {
    {"cancel", (PyCFunction)events__Event__cancel, METH_NOARGS,
        "cancel()\n\n"
        "Cancels a created event."},
    {"fire", (PyCFunction)events__Event__fire, METH_VARARGS,
        "fire([dont_broadcast=false])\n\n"
        "Fires a created event.\n\n"
        "@type  dont_broadcast: bool\n"
        "@param dont_broadcast: Determines whether or not to broadcast this event to\n"
        "   the clients."},
    {"get_bool", (PyCFunction)events__Event__get_bool, METH_VARARGS,
        "get_bool(field) -> bool\n\n"
        "Retrieves a boolean value from a game event.\n\n"
        "@type  field: str\n"
        "@param field: The field to access\n"
        "@rtype: bool\n"
        "@return: The value of a boolean field.\n"
        "@note: This will ALWAYS retrieve a value, even if the field does not exist.\n"
        "   Use has_field to make sure the field exists."},
    {"get_float", (PyCFunction)events__Event__get_float, METH_VARARGS,
        "get_float(field) -> float\n\n"
        "Retrieves a float value from a game event.\n\n"
        "@type  field: str\n"
        "@param field: The field to access\n"
        "@rtype: float\n"
        "@return: The value of a float field.\n"
        "@note: This will ALWAYS retrieve a value, even if the field does not exist.\n"
        "   Use has_field to make sure the field exists."},
    {"get_int", (PyCFunction)events__Event__get_int, METH_VARARGS,
        "get_int(field) -> int\n\n"
        "Retrieves an integer value from a game event.\n\n"
        "@type  field: str\n"
        "@param field: The field to access\n"
        "@rtype: int\n"
        "@return: The value of an integer field.\n"
        "@note: This will ALWAYS retrieve a value, even if the field does not exist.\n"
        "   Use has_field to make sure the field exists."},
    {"get_string", (PyCFunction)events__Event__get_string, METH_VARARGS,
        "get_string(field) -> str\n\n"
        "Retrieves a string value from a game event.\n\n"
        "@type  field: str\n"
        "@param field: The field to access\n"
        "@rtype: str\n"
        "@return: The value of a string field.\n"
        "@note: This will ALWAYS retrieve a value, even if the field does not exist.\n"
        "   Use has_field to make sure the field exists."},
    {"has_field", (PyCFunction)events__Event__has_field, METH_VARARGS,
        "has_field(field) -> bool\n\n"
        "Returns whether or not a field exists on an event.\n\n"
        "@type  field: str\n"
        "@param field: The field to check for existence\n"
        "@rtype: bool\n"
        "@return: True if the field exists, False if not."},
    {"set_bool", (PyCFunction)events__Event__set_bool, METH_VARARGS,
        "set_bool(field, value)\n\n"
        "Sets a boolean value in a game event.\n\n"
        "@type  field: str\n"
        "@param field: The field to set\n"
        "@type  value: bool\n"
        "@param value: The boolean value to set."},
    {"set_float", (PyCFunction)events__Event__set_float, METH_VARARGS,
        "set_float(field, value)\n\n"
        "Sets a float value in a game event.\n\n"
        "@type  field: str\n"
        "@param field: The field to set\n"
        "@type  value: float\n"
        "@param value: The float value to set."},
    {"set_int", (PyCFunction)events__Event__set_int, METH_VARARGS,
        "set_int(field, value)\n\n"
        "Sets an integer value in a game event.\n\n"
        "@type  field: str\n"
        "@param field: The field to set\n"
        "@type  value: int\n"
        "@param value: The int value to set."},
    {"set_string", (PyCFunction)events__Event__set_string, METH_VARARGS,
        "set_string(field, value)\n\n"
        "Sets a string value in a game event.\n\n"
        "@type  field: str\n"
        "@param field: The field to set\n"
        "@type  value: str\n"
        "@param value: The string value to set."},
    {NULL, NULL, 0, NULL},
};

PyTypeObject events__EventType = {
    PyObject_HEAD_INIT(&PyType_Type)
    0,                          /*ob_size*/
    "sourcemod.events.Event",   /*tp_name*/
    sizeof(events__Event),      /*tp_basicsize*/
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
    (reprfunc)events__Event__str__,/*tp_str*/
    0,                          /*tp_getattro*/
    0,                          /*tp_setattro*/
    0,                          /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT,         /*tp_flags*/
    /* tp_doc */
    "Contains methods to manipulate a game event.",
    0,		                    /* tp_traverse */
    0,		                    /* tp_clear */
    0,		                    /* tp_richcompare */
    0,		                    /* tp_weaklistoffset */
    0,		                    /* tp_iter */
    0,		                    /* tp_iternext */
    events__Event__methods,     /* tp_methods */
    events__Event__members,     /* tp_members */
    events__Event__getsets,     /* tp_getset */
    0,                          /* tp_base */
    0,                          /* tp_dict */
    0,                          /* tp_descr_get */
    0,                          /* tp_descr_set */
    0,                          /* tp_dictoffset */
};

static PyObject *
events__create(PyObject *self, PyObject *args)
{
    char const *name;
    bool force = false;
    
    if (!PyArg_ParseTuple(args, "s|b", &name, &force))
        return NULL;
    
    IGameEvent *pEvent = gameevents->CreateEvent(name, force);
    if (pEvent == NULL)
        Py_RETURN_NONE;
    
    events__Event *pyEvent = PyObject_New(events__Event, &events__EventType);
    pyEvent->event = pEvent;
    
    return (PyObject*)pyEvent;
}

static PyObject *
events__hook(PyObject *self, PyObject *args)
{
    char const *event;
    PyObject *callback;
    ViperEventHookMode mode = EventHookMode_Post;
    
    if (!PyArg_ParseTuple(args, "sO|i", &event, &callback, &mode))
        return NULL;
    
    if (!PyCallable_Check(callback))
    {
        PyErr_SetString(g_pViperException, "The function passed was not callable");
        return NULL;
    }
    
    GET_THREAD_PLUGIN();
    
    IViperPluginFunction *pFunc = CPluginFunction::CreatePluginFunction(callback,
        pPlugin);
    
    ViperEventHookError status = g_EventManager.HookEvent(event, pFunc, mode);
    delete pFunc;
    
    if (status > 0)
    {
        switch(status)
        {
        case EventHookErr_InvalidEvent:
            return PyErr_Format(g_pViperException, "Event '%s' does not exist",
                event);
        
        case EventHookErr_NotActive:
            return PyErr_Format(g_pViperException, "Event '%s' has no active hook",
                event);
        
        case EventHookErr_InvalidCallback:
            return PyErr_Format(g_pViperException, "Event '%s' does not fire the"
                "specified callback", event);
        };
    }
    
    Py_RETURN_NONE;
}

static PyObject *
events__unhook(PyObject *self, PyObject *args)
{
    char const *event;
    PyObject *callback;
    ViperEventHookMode mode = EventHookMode_Post;
    
    if (!PyArg_ParseTuple(args, "sO|i", &event, &callback, &mode))
        return NULL;
    
    if (!PyCallable_Check(callback))
    {
        PyErr_SetString(g_pViperException, "The function passed was not callable");
        return NULL;
    }
    
    GET_THREAD_PLUGIN();
    
    IViperPluginFunction *pFunc = CPluginFunction::CreatePluginFunction(callback,
        pPlugin);
    
    ViperEventHookError status = g_EventManager.UnhookEvent(event, pFunc, mode);
    delete pFunc;
    
    if (status > 0)
    {
        switch(status)
        {
        case EventHookErr_InvalidEvent:
            return PyErr_Format(g_pViperException, "Event '%s' does not exist",
                event);
        
        case EventHookErr_NotActive:
            return PyErr_Format(g_pViperException, "Event '%s' has no active hook",
                event);
        
        case EventHookErr_InvalidCallback:
            return PyErr_Format(g_pViperException, "Event '%s' does not fire the"
                "specified callback", event);
        };
    }
    
    Py_RETURN_NONE;
}

static PyMethodDef events__methods[] = {
    {"create", events__create, METH_VARARGS,
        "create(name[, force=false]) -> Event object\n\n"
        "Creates a game event.\n"
        "@type  name: str\n"
        "@param name: The name of the event to create\n"
        "@type  force: bool\n"
        "@param force: Forces event to be created, even it's not being hooked. Note that\n"
        "   this will not force it if the event doesn't exist at all.\n"
        "@rtype: sourcemod.events.Event\n"
        "@return: The Event object that represents the game event created, or None if\n"
        "   the event does not exist."},
    {"hook", events__hook, METH_VARARGS,
        "hook(event, callback[, mode=EventHookMode_Post])\n\n"
        "Hooks a game event. Throws an exception if the game event does not exist.\n"
        "@type  event: str\n"
        "@param event: The game event to hook, e.g., \"player_death\"\n"
        "@type  callback: callable\n"
        "@param callback: The function to call when the event is fired.\n"
        "@type  mode: EventHookMode constant\n"
        "@param mode: Changes when and how the hook will be fired:\n"
        "   - EventHookMode_Pre: callback fired before event is fired\n"
        "   - EventHookMode_Post: callback fired after event is fired\n"
        "   - EventHookMode_PostNoCopy: callback fired after event is fired, but event\n"
        "     data won't be copied.\n"
        "@throw ViperException: Event does not exist\n"
        "@throw ViperException: Invalid callback"},
    {"unhook", events__unhook, METH_VARARGS,
        "unhook(event, callback[, mode=EventHookMode_Post])\n\n"
        "Unhooks a callback from a game event.\n"
        "@see: sourcemod.events.hook\n"
        "@throw ViperException: the specified event does not exist, or the callback\n"
        "   was invalid."},
    {NULL, NULL, 0, NULL},
};

PyObject *
initevents(void)
{
    if (PyType_Ready(&events__EventType) < 0)
        return NULL;
    
    PyObject *events = Py_InitModule3("events", events__methods,
        "Hooks and handles game events.");
    
    Py_INCREF((PyObject*)&events__EventType);
    PyModule_AddObject(events, "Event", (PyObject*)&events__EventType);
    
    PyModule_AddIntMacro(events, EventHookMode_Pre);
    PyModule_AddIntMacro(events, EventHookMode_Post);
    PyModule_AddIntMacro(events, EventHookMode_PostNoCopy);
    
    return events;
}

