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

#include "py_events.h"
#include "EventManager.h"
#include "PluginSys.h"
#include <structmember.h>
#include <bitbuf.h>

// winbase.h defines all of these as GetPropA/W (for UNICODE)
#undef CreateEvent

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
    if (!PyArg_ParseTuple(args, "|b", &dont_broadcast))
        return NULL;
    
    gameevents->FireEvent(self->event, dont_broadcast);
    
    /* Invalidate this object */
    self->event = NULL;
    
    Py_RETURN_NONE;
}

/* This returns true only when the field has a value. */
static PyObject *
events__Event__is_empty(events__Event *self, PyObject *args)
{
    if (self->event == NULL)
    {
        PyErr_SetString(g_pViperException, "Invalid game event.");
        return NULL;
    }
    
    char const *field;
    if (!PyArg_ParseTuple(args, "s", &field))
        return NULL;
    
    return Py_BuildValue("b", self->event->IsEmpty(field));
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

static PyObject *
events__Event__subscript__(events__Event *self, PyObject *key)
{
    if (!PyString_Check(key))
        return PyErr_Format(_PyExc_TypeError, "expected key of type string, not "
            "%s.", key->ob_type->tp_name);
    
    if (self->event == NULL)
    {
        PyErr_SetString(g_pViperException, "Invalid game event.");
        return NULL;
    }
    
    char const *keystr = PyString_AS_STRING(key);
    if (self->event->IsEmpty(keystr))
    {
        PyErr_Format(g_pViperException, "field \"%s\" does not exist in "
            "game event \"%s\".", keystr, self->event->GetName());
        return NULL;
    }
    
    /* I do not yet know why, but the first time execution reaches
     * this function, self->fields is set to 0x1. So at the cost of
     * a trie lookup every call, we'll fix that.
     */
    self->fields = g_EventManager.GetEventFields(self->event->GetName());
    if (self->fields == NULL)
    {
        PyErr_Format(g_pViperException, "UH-OH! Could not retrieve field "
            " type list for game event \"%s\".", self->event->GetName());
        return NULL;
    }
    
    switch (g_EventManager.GetFieldType(self->fields, keystr))
    {
    case ModEventType_Local:
    case ModEventType_String:
        return PyString_FromString(self->event->GetString(keystr));
    
    case ModEventType_Bool:
        return PyBool_FromLong(self->event->GetBool(keystr));
    
    case ModEventType_Byte:
    case ModEventType_Short:
    case ModEventType_Long:
        return PyInt_FromLong(self->event->GetInt(keystr));
    
    case ModEventType_Float:
        return PyFloat_FromDouble(self->event->GetFloat(keystr));
    
    default:
        Py_RETURN_NONE;
    };
}

static int
events__Event__ass_subscript__(events__Event *self, PyObject *key,
                               PyObject *value)
{
    if (!PyString_Check(key))
    {
        PyErr_Format(_PyExc_TypeError, "expected key of type string, not %s.",
            key->ob_type->tp_name);
        return -1;
    }
    
    if (self->event == NULL)
    {
        PyErr_SetString(g_pViperException, "Invalid game event.");
        return NULL;
    }
    
    //if (self->fields == NULL)
    //{
        self->fields = g_EventManager.GetEventFields(self->event->GetName());
        if (self->fields == NULL)
        {
            PyErr_Format(g_pViperException, "UH-OH! Could not retrieve field "
                " type list for game event \"%s\".", self->event->GetName());
            return -1;
        }
    //}
    
    char const *keystr = PyString_AS_STRING(key);
    ModEventType type = g_EventManager.GetFieldType(self->fields, keystr);
    
    switch (type)
    {
    case ModEventType_Local:
    case ModEventType_String:
        if (!PyString_Check(value))
        {
            PyErr_Format(_PyExc_TypeError, "expected field \"%s\" value of "
                "type string, not %s.", keystr, value->ob_type->tp_name);
            return -1;
        }
        
        self->event->SetString(keystr, PyString_AS_STRING(value));
        return 0;
    
    case ModEventType_Bool:
        if (!PyBool_Check(value))
        {
            PyErr_Format(_PyExc_TypeError, "expected field \"%s\" value of "
                "type bool, not %s.", keystr, value->ob_type->tp_name);
            return -1;
        }
        
        self->event->SetBool(keystr, value == Py_True);
        return 0;
    
    case ModEventType_Byte:
    case ModEventType_Short:
    case ModEventType_Long:
        if (!PyInt_Check(value))
        {
            PyErr_Format(_PyExc_TypeError, "expected field \"%s\" value of "
                "type int, not %s.", keystr, value->ob_type->tp_name);
            return -1;
        }
        
        self->event->SetInt(keystr, PyInt_AS_LONG(value));
        return 0;
    
    case ModEventType_Float:
        if (!PyFloat_Check(value))
        {
            PyErr_Format(_PyExc_TypeError, "expected field \"%s\" value of "
                "type float, not %s.", keystr, value->ob_type->tp_name);
            return -1;
        }
        
        self->event->SetFloat(keystr, PyFloat_AS_DOUBLE(value));
        return 0;
    
    default:
        // Execution should _never_ get here
        PyErr_Format(_PyExc_TypeError, "unknown field type %d encountered "
            "on event \"%s\" field \"%s\"", type, self->event->GetName(),
            keystr);
        return -1;
    }
    
    // sawce
    return 0;
}

static PyMappingMethods events__EventMappingMethods = {
    (lenfunc)NULL,                                   /*mp_length*/
    (binaryfunc)events__Event__subscript__,          /*mp_subscript*/
    (objobjargproc)events__Event__ass_subscript__    /*mp_ass_subscript*/
};

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
        "fire([dont_broadcast=False])\n\n"
        "Fires a created event.\n\n"
        "@type  dont_broadcast: bool\n"
        "@param dont_broadcast: Determines whether or not to broadcast this event to\n"
        "   the clients."},
    {"is_empty", (PyCFunction)events__Event__is_empty, METH_VARARGS,
        "is_empty(field) -> bool\n\n"
        "Returns whether or not an event field has an empty value.\n\n"
        "@type  field: str\n"
        "@param field: The field to check\n"
        "@rtype: bool\n"
        "@return: True if the field has a value, False if not."},
    {NULL, NULL, 0, NULL},
};

PyTypeObject events__EventType = {
    PyObject_HEAD_INIT(_PyType_Type)
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
    &events__EventMappingMethods,/*tp_as_mapping*/
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
    pyEvent->fields = NULL;
    
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
        "create(name[, force=False]) -> Event object\n\n"
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
    Py_INCREF(_PyType_Type);
    events__EventType.ob_type = _PyType_Type;
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

