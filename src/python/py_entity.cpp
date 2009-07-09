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

class CBaseEntity;

#include <Python.h>
#include "py_entity.h"
#include <IPlayerHelpers.h>
#include <edict.h>
#include <dt_shared.h>
#include <server_class.h>
#include <datamap.h>
#include "extension.h"

using SourceMod::IGamePlayer;
using SourceMod::sm_sendprop_info_t;

/* Pointer to SendPropEHandle for SendProp entity detection */
void *g_pSendProxy_EHandleToInt;

enum ViperPropType
{
    Prop_Send = 0,
    Prop_Data
};

enum ViperFieldType
{
    FIELDTYPE_AUTODETECT = -1,
    FIELDTYPE_BOOLEAN = 1, 
    FIELDTYPE_CHAR = 8,
    FIELDTYPE_SHORT = 16,
    FIELDTYPE_INTEGER = 32,
    FIELDTYPE_FLOAT,
    FIELDTYPE_ENTITY,
    FIELDTYPE_VECTOR,
    FIELDTYPE_STRING,
    FIELDTYPE_MAX
};

inline CBaseEntity *GetEntity(edict_t *pEdict)
{
    IServerUnknown *pUnk = pEdict->GetUnknown();
    if (pUnk == NULL)
        return NULL;
    
    return pUnk->GetBaseEntity();
}

inline char const *GetTypeString(ViperFieldType type)
{
    switch (type)
    {
    case FIELDTYPE_AUTODETECT:
        return "<none>";
    case FIELDTYPE_BOOLEAN:
        return "boolean";
    case FIELDTYPE_CHAR:
        return "character";
    case FIELDTYPE_SHORT:
        return "short";
    case FIELDTYPE_INTEGER:
        return "integer";
    case FIELDTYPE_FLOAT:
        return "float";
    case FIELDTYPE_ENTITY:
        return "entity";
    case FIELDTYPE_VECTOR:
        return "vector";
    case FIELDTYPE_STRING:
        return "string";
    default:
        return "<unknown>";
    }
}

struct entity__EntityProps {
    PyObject_HEAD
    
    entity__Entity *entity;
    ViperPropType type;
};

static PyObject *
entity__EntityProps__subscript(entity__EntityProps *self, PyObject *pyprop)
{
    /* TODO: ARRAYS!!! */
    PyObject *pypropstring = PyObject_Str(pyprop);
    char const *prop = PyString_AsString(pypropstring);
    Py_DECREF(pypropstring);
    
    if (prop == NULL)
    {
        PyErr_SetString(g_pViperException, "Key object passed could not be "
            "coerced into a string.");
        return NULL;
    }
    
    if (prop[0] == '\0')
    {
        PyErr_SetString(g_pViperException, "Entity properties cannot be empty.");
        return NULL;
    }
    
    int bit_count;
    int prop_offset;
    int size;
    ViperFieldType field_type = FIELDTYPE_AUTODETECT;
    CBaseEntity *pEntity = GetEntity(self->entity->edict);
    
    if (pEntity == NULL)
    {
        PyErr_Format(g_pViperException, "Entity %d is invalid.",
            gamehelpers->IndexOfEdict(self->entity->edict));
        return NULL;
    }
    
    char const *class_name = self->entity->edict->GetClassName();
    if (class_name == NULL)
        class_name = "";
    
    switch (self->type)
    {
    case Prop_Data:
        {
            typedescription_t *td;
            
            datamap_t *pMap;
            if ((pMap = gamehelpers->GetDataMap(pEntity)) == NULL)
            {
                PyErr_SetString(g_pViperException, "Could not retrieve datamap");
                return NULL;
            }
            
            if ((td = gamehelpers->FindInDataMap(pMap, prop)) == NULL)
            {
                PyErr_Format(g_pViperException, "Property \"%s\" not found (entity %d/%s)",
                    prop, gamehelpers->IndexOfEdict(self->entity->edict), class_name);
                return NULL;
            }
            
            prop_offset = td->fieldOffset[TD_OFFSET_NORMAL];
            
            switch (td->fieldType)
            {
            case FIELD_TICK:
            case FIELD_MODELINDEX:
            case FIELD_MATERIALINDEX:
            case FIELD_INTEGER:
            case FIELD_COLOR32:
                bit_count = 32;
                field_type = FIELDTYPE_INTEGER;
                break;
            case FIELD_SHORT:
                bit_count = 16;
                field_type = FIELDTYPE_INTEGER;
                break;
            case FIELD_CHARACTER:
                bit_count = 8;
                field_type = FIELDTYPE_INTEGER;
                break;
            case FIELD_BOOLEAN:
                bit_count = 1;
                field_type = FIELDTYPE_INTEGER;
                break;
            
            case FIELD_EHANDLE:
                field_type = FIELDTYPE_ENTITY;
                break;
            
            case FIELD_VECTOR:
            case FIELD_POSITION_VECTOR:
                field_type = FIELDTYPE_VECTOR;
            
            case FIELD_STRING:
            case FIELD_MODELNAME:
            case FIELD_SOUNDNAME:
                field_type = FIELDTYPE_STRING;
            
            default:
                PyErr_Format(g_pViperException, "Unable to autodetect the type "
                    "of data field %s (type %d)", prop, td->fieldType);
                return NULL;
            }
            
            break;
        }
    case Prop_Send:
        {
            sm_sendprop_info_t info;
            
            IServerNetworkable *pNet = self->entity->edict->GetNetworkable();
            if (pNet == NULL)
            {
                PyErr_Format(g_pViperException, "Edict %d is not networkable",
                    gamehelpers->IndexOfEdict(self->entity->edict));
                return NULL;
            }
            
            if (!gamehelpers->FindSendPropInfo(pNet->GetServerClass()->GetName(), prop, &info))
            {
                PyErr_Format(g_pViperException, "Property \"%s\" not found (entity %d/%s)",
                    prop, gamehelpers->IndexOfEdict(self->entity->edict), class_name);
                return NULL;
            }
            
            bit_count = info.prop->m_nBits;
            prop_offset = info.actual_offset;
            
            switch (info.prop->GetType())
            {
            case DPT_Int:
                if ((void*)info.prop->GetProxyFn() == g_pSendProxy_EHandleToInt)
                {
                    field_type = FIELDTYPE_ENTITY;
                    break;
                }
                
                field_type = FIELDTYPE_INTEGER;
                bit_count = info.prop->m_nBits;
                break;
            
            case DPT_Float:
                field_type = FIELDTYPE_FLOAT;
                break;
            
            case DPT_Vector:
                field_type = FIELDTYPE_VECTOR;
                break;
            
            case DPT_String:
                field_type = FIELDTYPE_STRING;
                break;
            
            default:
                PyErr_Format(g_pViperException, "Unable to autodetect the type "
                    "of SendProp %s (type %d)", prop, info.prop->GetType());
                return NULL;
            }
            
            break;
        }
    }
    
    switch (field_type)
    {
    case FIELDTYPE_INTEGER:
        {
            int value;
            
            if (bit_count < 1)
                bit_count = size * 8;
            
            if (bit_count >= 17)
                value = *(int32_t *)((uint8_t *)pEntity + prop_offset);
            else if (bit_count >= 9)
                value = *(int16_t *)((uint8_t *)pEntity + prop_offset);
            else if (bit_count >= 2)
                value = *(int8_t *)((uint8_t *)pEntity + prop_offset);
            else
                return PyBool_FromLong(*(bool *)((uint8_t *)pEntity + prop_offset) ? 1 : 0);
            
            return PyInt_FromLong(value);
        }
    
    case FIELDTYPE_FLOAT:
        return PyFloat_FromDouble(*(float *)((uint8_t *)pEntity + prop_offset));
    
    case FIELDTYPE_VECTOR:
        {
            Vector *vec = (Vector *)((uint8_t *)pEntity + prop_offset);
            
            /* tuple(float, float, float) */
            return PyTuple_Pack(3, PyFloat_FromDouble(vec->x),
                PyFloat_FromDouble(vec->y), PyFloat_FromDouble(vec->z));
        }
    
    case FIELDTYPE_STRING:
        {
            string_t idx = *(string_t *)((uint8_t *)pEntity + prop_offset);
            return PyString_FromString((idx == NULL_STRING) ? "" : STRING(idx));
        }
    
    case FIELDTYPE_ENTITY:
        {
            CBaseHandle &hndl = *(CBaseHandle *)((uint8_t *)pEntity + prop_offset);
            
            if (!hndl.IsValid())
                Py_RETURN_NONE;

            int index = hndl.GetEntryIndex();

            edict_t *pStoredEdict = gamehelpers->EdictOfIndex(index);
            CBaseEntity *pStoredEntity = GetEntity(pStoredEdict);
            if (pStoredEdict == NULL || pStoredEntity == NULL)
                Py_RETURN_NONE;

            IServerEntity *pSE = pStoredEdict->GetIServerEntity();
            if (pSE == NULL || pSE->GetRefEHandle() != hndl)
                Py_RETURN_NONE;
            
            entity__Entity *pyEntity = PyObject_New(entity__Entity, &entity__EntityType);
            pyEntity->edict = pStoredEdict;
                
            Py_INCREF((PyObject*)pyEntity);
            return (PyObject*)pyEntity;
        }
    
    default:
        Py_RETURN_NONE;    
    }
}

static int
entity__EntityProps__ass_subscript(entity__EntityProps *self, PyObject *pyprop,
                                   PyObject *setobj)
{
    /* TODO: ARRAYS!!! */
    PyObject *pypropstring = PyObject_Str(pyprop);
    char const *prop = PyString_AsString(pypropstring);
    Py_DECREF(pypropstring);
    
    if (prop == NULL)
    {
        PyErr_SetString(g_pViperException, "The key object requested could not "
            "be coerced into a string.");
        return -1;
    }
    
    if (prop[0] == '\0')
    {
        PyErr_SetString(g_pViperException, "Entity properties cannot be empty.");
        return -1;
    }
    
    int bit_count;
    int prop_offset;
    int size;
    ViperFieldType field_type = FIELDTYPE_AUTODETECT;
    CBaseEntity *pEntity = GetEntity(self->entity->edict);
    
    if (pEntity == NULL)
    {
        PyErr_Format(g_pViperException, "Entity %d is invalid.",
            gamehelpers->IndexOfEdict(self->entity->edict));
        return -1;
    }
    
    char const *class_name = self->entity->edict->GetClassName();
    if (class_name == NULL)
        class_name = "";
    
    switch (self->type)
    {
    case Prop_Data:
        {
            typedescription_t *td;
            
            datamap_t *pMap;
            if ((pMap = gamehelpers->GetDataMap(pEntity)) == NULL)
            {
                PyErr_SetString(g_pViperException, "Could not retrieve datamap");
                return -1;
            }
            if ((td = gamehelpers->FindInDataMap(pMap, prop)) == NULL)
            {
                PyErr_Format(g_pViperException, "Property \"%s\" not found (entity %d/%s)",
                    prop, gamehelpers->IndexOfEdict(self->entity->edict), class_name);
                return -1;
            }
            

            prop_offset = td->fieldOffset[TD_OFFSET_NORMAL];
            
            switch (td->fieldType)
            {
            case FIELD_TICK:
            case FIELD_MODELINDEX:
            case FIELD_MATERIALINDEX:
            case FIELD_INTEGER:
            case FIELD_COLOR32:
                bit_count = 32;
                field_type = FIELDTYPE_INTEGER;
                break;
            case FIELD_SHORT:
                bit_count = 16;
                field_type = FIELDTYPE_INTEGER;
                break;
            case FIELD_CHARACTER:
                bit_count = 8;
                field_type = FIELDTYPE_INTEGER;
                break;
            case FIELD_BOOLEAN:
                bit_count = 1;
                field_type = FIELDTYPE_INTEGER;
                break;
            
            case FIELD_EHANDLE:
                field_type = FIELDTYPE_ENTITY;
                break;
            
            case FIELD_VECTOR:
            case FIELD_POSITION_VECTOR:
                field_type = FIELDTYPE_VECTOR;
            
            case FIELD_STRING:
            case FIELD_MODELNAME:
            case FIELD_SOUNDNAME:
                field_type = FIELDTYPE_STRING;
            
            default:
                PyErr_Format(g_pViperException, "Unable to autodetect the type "
                    "of data field %s (type %d)", prop, td->fieldType);
                return -1;
            }
            
            break;
        }
    case Prop_Send:
        {
            sm_sendprop_info_t info;
            
            IServerNetworkable *pNet = self->entity->edict->GetNetworkable();
            if (pNet == NULL)
            {
                PyErr_Format(g_pViperException, "Edict %d (%s) is not networkable",
                    gamehelpers->IndexOfEdict(self->entity->edict), class_name);
                return -1;
            }
            
            if (!gamehelpers->FindSendPropInfo(pNet->GetServerClass()->GetName(), prop, &info))
            {
                PyErr_Format(g_pViperException, "Property \"%s\" not found (entity %d/%s)",
                    prop, gamehelpers->IndexOfEdict(self->entity->edict), class_name);
                return -1;
            }
            
            bit_count = info.prop->m_nBits;
            prop_offset = info.actual_offset;
            
            switch (info.prop->GetType())
            {
            case DPT_Int:
                if ((void*)info.prop->GetProxyFn() == g_pSendProxy_EHandleToInt)
                {
                    field_type = FIELDTYPE_ENTITY;
                    break;
                }
                
                field_type = FIELDTYPE_INTEGER;
                bit_count = info.prop->m_nBits;
                break;
            
            case DPT_Float:
                field_type = FIELDTYPE_FLOAT;
                break;
            
            case DPT_Vector:
                field_type = FIELDTYPE_VECTOR;
                break;
            
            case DPT_String:
                field_type = FIELDTYPE_STRING;
                break;
            
            default:
                PyErr_Format(g_pViperException, "Unable to autodetect the type "
                    "of SendProp %s (type %d)", prop, info.prop->GetType());
                return -1;
            }
            
            break;
        }
    }
    
    switch (field_type)
    {
    case FIELDTYPE_BOOLEAN:
    case FIELDTYPE_CHAR:
    case FIELDTYPE_SHORT:
    case FIELDTYPE_INTEGER:
        {
            if (bit_count == 1)
            {
                if (!PyBool_Check(setobj))
                {
                    PyErr_Format(PyExc_TypeError, "Expected bool, found '%s'",
                        setobj->ob_type->tp_name);
                    return -1;
                }
                
                *(bool *)((uint8_t *)pEntity + prop_offset) = (setobj == Py_True);
                
                return 0;
            }
            
            if (bit_count < 1)
                bit_count = size * 8;
            
            int value;
            if (!PyInt_Check(setobj))
            {
                PyErr_Format(PyExc_TypeError, "Expected int (%d bits), found '%s'",
                    bit_count, setobj->ob_type->tp_name);
                return -1;
            }
            
            value = PyInt_AS_LONG(setobj);
            
            if (bit_count >= 17)
                *(int32_t *)((uint8_t *)pEntity + prop_offset) = (int32_t)value;
            else if (bit_count >= 9)
                *(int16_t *)((uint8_t *)pEntity + prop_offset) = (int16_t)value;
            else if (bit_count >= 2)
                *(int8_t *)((uint8_t *)pEntity + prop_offset) = (int8_t)value;
            
            return 0;
        }
    
    case FIELDTYPE_FLOAT:
        {
            if (!PyFloat_Check(setobj))
            {
                PyErr_Format(PyExc_TypeError, "Expected float, found '%s'",
                    setobj->ob_type->tp_name);
                return -1;
            }
            
            *(float *)((uint8_t *)pEntity + prop_offset) = PyFloat_AS_DOUBLE(setobj);
            
            return 0;
        }
    
    case FIELDTYPE_VECTOR:
        {
            float x, y, z;
            
            /* Unpack the tuple as three floats. */
            if (!PyArg_ParseTuple(setobj, "fff", &x, &y, &z))
                return -1;
            
            Vector *vec = (Vector *)((uint8_t *)pEntity + prop_offset);
            vec->x = x;
            vec->y = y;
            vec->z = z;
            
            return 0;
        }
    
    case FIELDTYPE_STRING:
        {
            PyObject *str_setobj = PyObject_Str(setobj);
            if (str_setobj == NULL)
            {
                PyErr_SetString(PyExc_TypeError, "Value could not be coerced "
                    "into a string.");
                return -1;
            }
            
            string_t str = MAKE_STRING(sm_strdup(PyString_AS_STRING(str_setobj)));
            Py_DECREF(str_setobj);
            
            *(string_t *)((uint8_t *)pEntity + prop_offset) = str;
            return 0;
        }
    
    case FIELDTYPE_ENTITY:
        {
            CBaseHandle &hndl = *(CBaseHandle *)((uint8_t *)pEntity + prop_offset);
            
            if (setobj == Py_None)
            {
                hndl.Set(NULL);
                return 0;
            }
            
            if (!PyObject_IsInstance(setobj, (PyObject *)&entity__EntityType))
            {
                PyErr_Format(PyExc_TypeError, "Expected entity.Entity, found '%s'",
                    setobj->ob_type->tp_name);
                return -1;
            }
            
            edict_t *edict = ((entity__Entity *)setobj)->edict;
            hndl.Set(edict->GetIServerEntity());
            
            if (self->type == Prop_Send)
                gamehelpers->SetEdictStateChanged(self->entity->edict, prop_offset);
            
            return 0;
        }
    
    default:
        PyErr_Format(g_pViperException, "Unable to set the value of the entity "
            "property \"%s\" of type %s", prop, GetTypeString(field_type));
        return -1;
    }
    
    return 0;
}

static PyObject *
entity__EntityProps__str__(entity__EntityProps *self)
{
    PyObject *entobj_string = PyObject_Str((PyObject*)self->entity);
    
    PyObject *strobj = PyString_FromFormat("<%s EntityProps for %s>",
        self->type == Prop_Send ? "SendProps" : "Datamaps",
        PyString_AS_STRING(entobj_string));
    
    Py_DECREF(entobj_string);
    
    return strobj;
}

PyMappingMethods entity__EntityPropsMappingMethods = {
    0,                          /* mp_length */
    (binaryfunc)entity__EntityProps__subscript,/* mp_subscript */
    (objobjargproc)entity__EntityProps__ass_subscript,/* mp_ass_subscript */
};

PyTypeObject entity__EntityPropsType = {
    PyObject_HEAD_INIT(&PyType_Type)
    0,                          /*ob_size*/
    "sourcemod.entity.EntityProps",/*tp_name*/
    sizeof(entity__EntityProps),/*tp_basicsize*/
    0,                          /*tp_itemsize*/
    0,                          /*tp_dealloc*/
    0,                          /*tp_print*/
    0,                          /*tp_getattr*/
    0,                          /*tp_setattr*/
    0,                          /*tp_compare*/
    0,                          /*tp_repr*/
    0,                          /*tp_as_number*/
    0,                          /*tp_as_sequence*/
    &entity__EntityPropsMappingMethods,/*tp_as_mapping*/
    0,                          /*tp_hash */
    0,                          /*tp_call*/
    (reprfunc)entity__EntityProps__str__,/*tp_str*/
    0,                          /*tp_getattro*/
    0,                          /*tp_setattro*/
    0,                          /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT,         /*tp_flags*/
    /* tp_doc */
    "Retrieves sendprops and datamaps from an entity.",
};

static PyObject *
entity__Entity__is_networkable(entity__Entity *self)
{
    return PyBool_FromLong((self->edict->GetNetworkable() != NULL) ? 1 : 0);
}

static PyObject *
entity__Entity__is_valid(entity__Entity *self)
{
    IServerUnknown *pUnknown = self->edict->GetUnknown();
    if (pUnknown == NULL)
        Py_RETURN_FALSE;

    CBaseEntity *pEntity = pUnknown->GetBaseEntity();
    return PyBool_FromLong((pEntity != NULL) ? 1 : 0);
}

static int
entity__Entity__cmp__(entity__Entity *self, PyObject *other)
{
    int isinstance = PyObject_IsInstance(other, (PyObject*)&entity__EntityType);
    if (isinstance == 0)
    {
        PyErr_Format(PyExc_TypeError, "Comparison using wrong type '%s'",
            other->ob_type->tp_name);
        return -1;
    }
    else if (isinstance == -1)
        return -1;
    
    int selfidx = gamehelpers->IndexOfEdict(self->edict);
    int otheridx = gamehelpers->IndexOfEdict(((entity__Entity *)other)->edict);
    
    if (selfidx == -1 || otheridx == -1)
        return self->edict - ((entity__Entity *)other)->edict;
    
    return selfidx - otheridx;
}

static void
entity__Entity__del__(entity__Entity *self)
{
    Py_CLEAR(self->sendprops);
    Py_CLEAR(self->datamaps);
    
    ((PyObject *)self)->ob_type->tp_free((PyObject *)self);
}

static int
entity__Entity__init__(entity__Entity *self, PyObject *args, PyObject *keywds)
{
    /* Accept an entity index, if it's valid */
    int entindex;
    if (!PyArg_ParseTuple(args, "i", &entindex))
        return -1;
    
    edict_t *pEdict = gamehelpers->EdictOfIndex(entindex);
    if (pEdict == NULL)
    {
        PyErr_Format(g_pViperException, "Entity index %d is invalid.", entindex);
        return -1;
    }
    
    self->edict = pEdict;
    
    /* Kill the properties objects, so they will work even if some idjit like
     * sawce calls __init__ twice.
     */
    self->sendprops = NULL;
    self->datamaps = NULL;
    
    return 0;
}

static PyObject *
entity__Entity__new__(PyTypeObject *type, PyObject *args, PyObject *keywds)
{
    entity__Entity *ent = (entity__Entity *)type->tp_alloc(type, 1);
    ent->sendprops = NULL;
    ent->datamaps = NULL;
    
    return (PyObject *)ent;
}

static PyObject *
entity__Entity__str__(entity__Entity *self)
{
    const char *cls = self->edict->GetClassName();
    if (cls == NULL || cls[0] == '\0')
        return PyString_FromFormat("<Entity %d at: %p>",
            gamehelpers->IndexOfEdict(self->edict), self->edict);
    
    return PyString_FromFormat("<Entity %d: '%s' at: %p>",
        gamehelpers->IndexOfEdict(self->edict), cls, self->edict);
}

static int
entity__Entity__traverse__(entity__Entity *self, visitproc visit, void *arg)
{
    Py_VISIT(self->sendprops);
    Py_VISIT(self->datamaps);
    
    return 0;
}

static PyObject *
entity__Entity__classnameget(entity__Entity *self, void *closure)
{
    const char *cls = self->edict->GetClassName();
    
    if (cls == NULL || cls[0] == '\0')
        Py_RETURN_NONE;
    
    return PyString_FromString(cls);
}

static PyObject *
entity__Entity__edict_flagsget(entity__Entity *self, void *closure)
{
    return PyInt_FromLong(self->edict->m_fStateFlags);
}

static int
entity__Entity__edict_flagsset(entity__Entity *self, PyObject *set, void *closure)
{
    if (!PyInt_Check(set))
    {
        PyErr_Format(PyExc_TypeError, "Edict flags must be set to an integer. "
            "Found type '%s'", set->ob_type->tp_name);
        return -1;
    }
    
    self->edict->m_fStateFlags = PyInt_AS_LONG(set);
    return 0;
}

static PyObject *
entity__Entity__datamapsget(entity__Entity *self, void *closure)
{
    if (self->datamaps == NULL)
    {
        self->datamaps = PyObject_New(entity__EntityProps, &entity__EntityPropsType);
        self->datamaps->entity = self;
        self->datamaps->type = Prop_Data;
        
        Py_INCREF(self->datamaps);
    }
    
    return (PyObject*)self->datamaps;
}

static PyObject *
entity__Entity__netclassget(entity__Entity *self, void *closure)
{
    IServerNetworkable *pNet = self->edict->GetNetworkable();
    if (pNet == NULL)
        Py_RETURN_NONE;

    ServerClass *pClass = pNet->GetServerClass();
    
    return PyString_FromString(pClass->GetName());
}

static PyObject *
entity__Entity__sendpropsget(entity__Entity *self, void *closure)
{
    if (self->sendprops == NULL)
    {
        self->sendprops = PyObject_New(entity__EntityProps, &entity__EntityPropsType);
        self->sendprops->entity = self;
        self->sendprops->type = Prop_Send;
        
        Py_INCREF(self->sendprops);
    }
    
    return (PyObject*)self->sendprops;
}

static PyGetSetDef entity__Entity__getsets[] = {
    {"classname", (getter)entity__Entity__classnameget, NULL,
        "This edict's classname."},
    {"datamaps", (getter)entity__Entity__datamapsget, NULL,
        "Datamaps for this Entity."},
    {"edict_flags", (getter)entity__Entity__edict_flagsget, (setter)entity__Entity__edict_flagsset,
        "The flags of the edict. These are not the same as entity flags."},
    {"netclass", (getter)entity__Entity__netclassget, NULL,
        "This entity's networkable serverclass name. This is not the same as the "
        "classname and is used for networkable state changes."},
    {"sendprops", (getter)entity__Entity__sendpropsget, NULL,
        "SendProps for this Entity."},
    {NULL},
};

static PyMethodDef entity__Entity__methods[] = {
#if NOT_DOCUMENTED_YET
    {"RemoveEdict",             RemoveEdict},
#endif
#if NOT_IMPLEMENTED_YET
    {"change_edict_state", (PyCFunction)entity__Entity__change_edict_state, METH_VARARGS,
        "change_edict_state(offset=0)\n\n"
        "Marks an entity as state changed. This can be useful if you set an offset and\n"
        "wish for it to be immediately changed over the network. By default this is not\n"
        "done for offset setting functions.\n"},
#endif
    {"is_networkable", (PyCFunction)entity__Entity__is_networkable, METH_NOARGS,
        "is_networkable() -> bool\n\n"
        "Returns whether or not the entity is networkable.\n"
        "@rtype: bool\n"
        "@return: True if networkable, false otherwise."},
    {"is_valid", (PyCFunction)entity__Entity__is_valid, METH_NOARGS,
        "is_valid() -> bool\n\n"
        "Returns whether or not the entity is valid.\n"
        "@rtype: bool\n"
        "@return: True if valid, false otherwise."},
    {NULL, NULL, 0, NULL}
};

PyTypeObject entity__EntityType = {
    PyObject_HEAD_INIT(NULL)
    0,                          /*ob_size*/
    "sourcemod.entity.Entity",  /*tp_name*/
    sizeof(entity__Entity),     /*tp_basicsize*/
    0,                          /*tp_itemsize*/
    (destructor)entity__Entity__del__,/*tp_dealloc*/
    0,                          /*tp_print*/
    0,                          /*tp_getattr*/
    0,                          /*tp_setattr*/
    (cmpfunc)entity__Entity__cmp__,/*tp_compare*/
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
    Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE|Py_TPFLAGS_HAVE_GC,/*tp_flags*/
    /* tp_doc */
    "Contains methods and members to manipulate an edict and the entity it describes.",
    (traverseproc)entity__Entity__traverse__,/* tp_traverse */
    0,                          /* tp_clear */
    0,                          /* tp_richcompare */
    0,                          /* tp_weaklistoffset */
    0,                          /* tp_iter */
    0,                          /* tp_iternext */
    entity__Entity__methods,    /* tp_methods */
    0,                          /* tp_members */
    entity__Entity__getsets,    /* tp_getset */
    0,                          /* tp_base */
    0,                          /* tp_dict */
    0,                          /* tp_descr_get */
    0,                          /* tp_descr_set */
    0,                          /* tp_dictoffset */
    (initproc)entity__Entity__init__,/* tp_init */
    0,                          /* tp_alloc */
    entity__Entity__new__,      /* tp_new */
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
#if NOT_DOCUMENTED_YET
    {"CreateEdict",},
#endif
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
    if (PyType_Ready(&entity__EntityType) < 0 ||
        PyType_Ready(&entity__EntityPropsType) < 0)
        return NULL;
    
    PyObject *entity = Py_InitModule3("entity", entity__methods,
        "Contains functions and objects to manipulate entities.");
    
    Py_INCREF((PyObject*)&entity__EntityType);
    Py_INCREF((PyObject*)&entity__EntityPropsType);
    PyModule_AddObject(entity, "Entity", (PyObject*)&entity__EntityType);
    PyModule_AddObject(entity, "EntityProps", (PyObject*)&entity__EntityPropsType);
    
    return entity;
}

