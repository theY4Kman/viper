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
#include "extension.h"
#include <IPlayerHelpers.h>
#include <edict.h>

using SourceMod::IGamePlayer;

enum ViperPropType
{
	Prop_Send = 0,
	Prop_Data
};

/* GetEdict used from SourceMod
 * Copyright (C) 2004-2008 AlliedModders LLC.  All rights reserved.
 */
inline edict_t *GetEdict(int num)
{
    edict_t *pEdict = PEntityOfEntIndex(num);
    if (!pEdict || pEdict->IsFree())
        return NULL;
    
    if (num > 0 && num <= playerhelpers->GetMaxClients())
    {
        IGamePlayer *pPlayer = playerhelpers->GetGamePlayer(num);
        if (!pPlayer || !pPlayer->IsConnected())
            return NULL;
    }
    
    return pEdict;
}

typedef struct {
    PyObject_HEAD
    
    edict_t *edict;
} entity__Entity;

static PyObject *
entity__Entity__is_ent_valid(entity__Entity *self)
{
    Py_RETURN_NONE;
}

static PyObject *
entity__Entity__str__(entity__Entity *self)
{
    Py_RETURN_NONE;
}

static PyMethodDef entity__Entity__methods[] = {
#if NOT_DOCUMENTED_YET
    {"get_prop"},
    {"get_prop_ent"},
    {"get_prop_float"},
    {"get_prop_string"},
    {"get_prop_vector"},
    {"get_prop_array"},
    {"set_prop"},
    {"set_prop_ent"},
    {"set_prop_float"},
    {"set_prop_string"},
    {"set_prop_vector"},
    {"set_prop_array"},
    {""},
    {""},
    {""},
    {""},
    {""},
    {""},
    {"is_networkable"},
    {"get_net_class"},
    
	{"IsValidEdict",			IsValidEdict},
	{"RemoveEdict",				RemoveEdict},
	{"SetEdictFlags",			SetEdictFlags},
	
	{"ChangeEdictState", },
	{"CreateEdict",},
	{"FindDataMapOffs",			FindDataMapOffs},
	{"FindSendPropInfo",		FindSendPropInfo},
	{"FindSendPropOffs",		FindSendPropOffs},
	{"GetEdictClassname",		GetEdictClassname},
	{"GetEdictFlags",			GetEdictFlags},
#endif
    {"is_ent_valid", (PyCFunction)entity__Entity__is_ent_valid, METH_NOARGS,
        "is_ent_valid() -> bool\n\n"
        "Returns whether or not the entity is valid.\n"
        "@rtype: bool\n"
        "@return: True if valid, false otherwise."},
    {NULL, NULL, 0, NULL}
};

PyTypeObject entity__EntityType = {
    PyObject_HEAD_INIT(&PyType_Type)
    0,                          /*ob_size*/
    "sourcemod.entity.Entity", /*tp_name*/
    sizeof(entity__Entity),   /*tp_basicsize*/
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
    (reprfunc)entity__Entity__str__,/*tp_str*/
    0,                          /*tp_getattro*/
    0,                          /*tp_setattro*/
    0,                          /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT,         /*tp_flags*/
    /* tp_doc */
    "Contains methods and members to manipulate an edict and the entity it describes.",
    0,                          /* tp_traverse */
    0,                          /* tp_clear */
    0,                          /* tp_richcompare */
    0,                          /* tp_weaklistoffset */
    0,                          /* tp_iter */
    0,                          /* tp_iternext */
    entity__Entity__methods,    /* tp_methods */
    0,                          /* tp_members */
    0,                          /* tp_getset */
    0,                          /* tp_base */
    0,                          /* tp_dict */
    0,                          /* tp_descr_get */
    0,                          /* tp_descr_set */
    0,                          /* tp_dictoffset */
    0,                          /* tp_init */
    0,                          /* tp_alloc */
    0,                          /* tp_new */
};

static PyObject *
entity__get_entity_count(PyObject *self)
{
    return PyInt_FromLong(engine->GetEntityCount());
}

static PyObject *
entity__get_max_entities(PyObject *self)
{
    return PyInt_FromLong(g_SMAPI->pGlobals()->maxEntities);
}

static PyMethodDef entity__methods[] = {
    {"get_entity_count", (PyCFunction)entity__get_entity_count, METH_NOARGS,
        "get_entity_count() -> int\n\n"
        "Returns the number of entities in the server\n"
        "@rtype: int\n"
        "@return: Number of entities in the server."},
    {"get_max_entities", (PyCFunction)entity__get_max_entities, METH_NOARGS,
        "get_max_entities() -> int\n\n"
        "Returns the maximum number of entities.\n"
        "@rtype: int\n"
        "@return: Maximum number of entities."},
    {NULL, NULL, 0, NULL}
};

PyObject *
initentity(void)
{
    PyObject *entity = Py_InitModule3("entity", NULL,
        "Contains functions and objects to manipulate entities.");
    
    PyModule_AddIntMacro(entity, Prop_Send);
    PyModule_AddIntMacro(entity, Prop_Data);
    
    return entity;
}

