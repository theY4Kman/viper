#include "BoostPythonSM.h"
#include "EntityModule.h"
#include "sdk/smsdk_ext.h"
#include "Macros.h"
#include "ViperExtension.h"
#include "InvalidEdictExceptionType.h"
#include "InvalidEntityExceptionType.h"
#include "EntityNotNetworkableExceptionType.h"
#include "CompatWrappers.h"
#include "EntityPropertyFieldTypes.h"
#include "EntitySendPropertyInfoType.h"
#include "EntityDataMapInfoType.h"
#include "InvalidSendPropertyExceptionType.h"
#include "InvalidDataMapExceptionType.h"
#include "InvalidDataMapPropertyExceptionType.h"
#include "EntityOffsetOutOfRangeExceptionType.h"
#include "VectorType.h"
#include "server_class.h"
#include "dt_common.h"
#include "Util.h"

namespace py = boost::python;

#ifdef GetClassName
#undef GetClassName
#endif

int entity__get_max_entities() {
	return g_Interfaces.GlobalVarsInstance->maxEntities;
}

int entity__get_entity_count() {
	return engine->GetEntityCount();
}

bool entity__is_valid_entity(int entityIndex) {
	edict_t *edict = PEntityOfEntIndex(entityIndex);

	if(edict == NULL || edict->IsFree()) {
		return false;
	}

	CBaseEntity *baseEntity = g_Interfaces.ServerGameEntsInstance->EdictToBaseEntity(edict);

	return baseEntity != NULL;
}

bool entity__is_valid_edict(int edictIndex) {
	edict_t *edict = PEntityOfEntIndex(edictIndex);

	if(edict == NULL || edict->IsFree()) {
		return false;
	}

	return true;
}

bool entity__is_entity_networkable(int entityIndex) {
	edict_t *edict = PEntityOfEntIndex(entityIndex);

	if(edict == NULL || edict->IsFree()) {
		throw InvalidEdictExceptionType(entityIndex);
	}

	IServerNetworkable *networkable = edict->GetNetworkable();

	return networkable != NULL;
}

int entity__create_edict() {
	edict_t *edict = engine->CreateEdict();

	if(edict == NULL || edict->IsFree()) {
		return -1;
	}

	return IndexOfEdict(edict);
}

void entity__remove_edict(int edictIndex) {
	edict_t *edict = PEntityOfEntIndex(edictIndex);

	if(edict == NULL || edict->IsFree()) {
		throw InvalidEdictExceptionType(edictIndex);
	}

	engine->RemoveEdict(edict);
}

int entity__get_edict_flags(int edictIndex) {
	edict_t *edict = PEntityOfEntIndex(edictIndex);

	if(edict == NULL || edict->IsFree()) {
		throw InvalidEdictExceptionType(edictIndex);
	}

	return edict->m_fStateFlags;
}

void entity__set_edict_flags(int edictIndex, int edictFlags) {
	edict_t *edict = PEntityOfEntIndex(edictIndex);

	if(edict == NULL || edict->IsFree()) {
		throw InvalidEdictExceptionType(edictIndex);
	}

	edict->m_fStateFlags = edictFlags;
}

std::string entity__get_edict_class_name(int edictIndex) {
	edict_t *edict = PEntityOfEntIndex(edictIndex);

	if(edict == NULL || edict->IsFree()) {
		throw InvalidEdictExceptionType(edictIndex);
	}

	const char *classNameCStr = edict->GetClassName();
	
	std::string className = classNameCStr == NULL ? std::string() : std::string(classNameCStr);

	return className;
}

std::string entity__get_entity_net_class(int entityIndex) {
	edict_t *edict = PEntityOfEntIndex(entityIndex);

	if(edict == NULL || edict->IsFree()) {
		throw InvalidEdictExceptionType(entityIndex);
	}

	CBaseEntity *baseEntity = g_Interfaces.ServerGameEntsInstance->EdictToBaseEntity(edict);
	
	if(NULL == baseEntity) {
		throw InvalidEntityExceptionType(entityIndex);
	}

	IServerNetworkable *serverNetworkable = edict->GetNetworkable();

	if(NULL == serverNetworkable) {
		throw EntityNotNetworkableExceptionType(entityIndex);
	}

	ServerClass *serverClass = serverNetworkable->GetServerClass();

	return serverClass->GetName();
}

void entity__change_edict_state_(edict_t *edict, int offset) {
	#if SOURCE_ENGINE == SE_DARKMESSIAH
	edict->m_fStateFlags |= FL_EDICT_CHANGED;
#else
	if(NULL == g_Interfaces.SharedEdictChangeInfoInstance) {
		edict->m_fStateFlags |= FL_EDICT_CHANGED;
		return;
	}
	
	if(offset) {
		edict->StateChanged(offset);
		return;
	}
	
	edict->StateChanged();
#endif
}

void entity__change_edict_state(int edictIndex, int offset) {
	edict_t *edict = PEntityOfEntIndex(edictIndex);

	if(edict == NULL || edict->IsFree()) {
		throw InvalidEdictExceptionType(edictIndex);
	}

	entity__change_edict_state_(edict, offset);
}

int entity__find_send_property_offset(std::string className, std::string propertyName) {
	SendProp *sendProp = gamehelpers->FindInSendTable(className.c_str(), propertyName.c_str());

	if(NULL == sendProp) {
		throw InvalidSendPropertyExceptionType(className, propertyName);
	}

	return sendProp->GetOffset();
}

EntitySendPropertyInfoType entity__find_send_property_info(std::string className, std::string propertyName) {
	SourceMod::sm_sendprop_info_t info;

	bool found = gamehelpers->FindSendPropInfo(className.c_str(), propertyName.c_str(), &info);

	if(!found) {
		throw InvalidSendPropertyExceptionType(className, propertyName);
	}

	EntityPropertyFieldTypes fieldType;
	
	switch (info.prop->GetType()) {
		case DPT_Int:
			fieldType = PropField_Integer;
			break;
		case DPT_Float:
			fieldType = PropField_Float;
			break;
		case DPT_String:
			fieldType = PropField_String;
			break;
		case DPT_Vector:
			fieldType = PropField_Vector;
			break;
		default:
			fieldType = PropField_Unsupported;
			break;
	}

	return EntitySendPropertyInfoType(fieldType, info.prop->m_nBits, info.prop->GetOffset(), info.actual_offset);
}

EntityDataMapInfoType entity__find_data_map_info(int entityIndex, std::string propertyName) {
	edict_t *edict = PEntityOfEntIndex(entityIndex);

	if(edict == NULL || edict->IsFree()) {
		throw InvalidEdictExceptionType(entityIndex);
	}
	
	CBaseEntity *baseEntity = g_Interfaces.ServerGameEntsInstance->EdictToBaseEntity(edict);

	if(NULL == baseEntity) {
		throw InvalidEntityExceptionType(entityIndex);
	}

	datamap_t *dataMap = gamehelpers->GetDataMap(baseEntity);

	if(NULL == dataMap) {
		throw InvalidDataMapExceptionType(entityIndex);
	}

	typedescription_t * typeDescription = gamehelpers->FindInDataMap(dataMap, propertyName.c_str());

	if(NULL == typeDescription) {
		throw InvalidDataMapPropertyExceptionType(entityIndex, propertyName);
	}

	EntityPropertyFieldTypes fieldType;
	unsigned int numBits;

	switch (typeDescription->fieldType) {
		case FIELD_TICK:
		case FIELD_MODELINDEX:
		case FIELD_MATERIALINDEX:
		case FIELD_INTEGER:
		case FIELD_COLOR32:
			fieldType = PropField_Integer;
			numBits = 32;
			break;

		case FIELD_VECTOR:
		case FIELD_POSITION_VECTOR:
			fieldType = PropField_Vector;
			numBits = 12;
			break;

		case FIELD_SHORT:
			fieldType = PropField_Integer;
			numBits = 16;
			break;

		case FIELD_BOOLEAN:
			fieldType = PropField_Integer;
			numBits = 1;
			break;

		case FIELD_CHARACTER:
			if (typeDescription->fieldSize == 1) {
				fieldType = PropField_Integer;
				numBits = 8;
			}
			else {
				fieldType = PropField_String;
				numBits = 8 * typeDescription->fieldSize;
			}
			break;

		case FIELD_MODELNAME:
		case FIELD_SOUNDNAME:
		case FIELD_STRING:
			numBits = sizeof(string_t);
			fieldType = PropField_String_T;
			break;

		case FIELD_FLOAT:
		case FIELD_TIME:
			fieldType = PropField_Float;
			numBits = 32;
			break;

		case FIELD_EHANDLE:
			fieldType = PropField_Entity;
			numBits = 32;
			break;

		default:
			fieldType = PropField_Unsupported;
			numBits = 0;
	}

	return EntityDataMapInfoType(fieldType, numBits, GetTypeDescOffs(typeDescription));
}

int entity__get_entity_int(int entityIndex, int offset) {
	edict_t *edict = PEntityOfEntIndex(entityIndex);

	if(edict == NULL || edict->IsFree()) {
		throw InvalidEdictExceptionType(entityIndex);
	}
	
	CBaseEntity *baseEntity = g_Interfaces.ServerGameEntsInstance->EdictToBaseEntity(edict);

	if(NULL == baseEntity) {
		throw InvalidEntityExceptionType(entityIndex);
	}

	if (offset <= 0 || offset > 32768) {
		throw EntityOffsetOutOfRangeExceptionType(offset);
	}

	return *(int *)((uint8_t *)baseEntity + offset);
}

unsigned int entity__get_entity_unsigned_int(int entityIndex, int offset) {
	edict_t *edict = PEntityOfEntIndex(entityIndex);

	if(edict == NULL || edict->IsFree()) {
		throw InvalidEdictExceptionType(entityIndex);
	}
	
	CBaseEntity *baseEntity = g_Interfaces.ServerGameEntsInstance->EdictToBaseEntity(edict);

	if(NULL == baseEntity) {
		throw InvalidEntityExceptionType(entityIndex);
	}

	if (offset <= 0 || offset > 32768) {
		throw EntityOffsetOutOfRangeExceptionType(offset);
	}

	return *(unsigned int *)((uint8_t *)baseEntity + offset);
}

short entity__get_entity_short(int entityIndex, int offset) {
	edict_t *edict = PEntityOfEntIndex(entityIndex);

	if(edict == NULL || edict->IsFree()) {
		throw InvalidEdictExceptionType(entityIndex);
	}
	
	CBaseEntity *baseEntity = g_Interfaces.ServerGameEntsInstance->EdictToBaseEntity(edict);

	if(NULL == baseEntity) {
		throw InvalidEntityExceptionType(entityIndex);
	}

	if (offset <= 0 || offset > 32768) {
		throw EntityOffsetOutOfRangeExceptionType(offset);
	}

	return *(short *)((uint8_t *)baseEntity + offset);
}

unsigned short entity__get_entity_unsigned_short(int entityIndex, int offset) {
	edict_t *edict = PEntityOfEntIndex(entityIndex);

	if(edict == NULL || edict->IsFree()) {
		throw InvalidEdictExceptionType(entityIndex);
	}
	
	CBaseEntity *baseEntity = g_Interfaces.ServerGameEntsInstance->EdictToBaseEntity(edict);

	if(NULL == baseEntity) {
		throw InvalidEntityExceptionType(entityIndex);
	}

	if (offset <= 0 || offset > 32768) {
		throw EntityOffsetOutOfRangeExceptionType(offset);
	}

	return *(unsigned short *)((uint8_t *)baseEntity + offset);
}

char entity__get_entity_char(int entityIndex, int offset) {
	edict_t *edict = PEntityOfEntIndex(entityIndex);

	if(edict == NULL || edict->IsFree()) {
		throw InvalidEdictExceptionType(entityIndex);
	}
	
	CBaseEntity *baseEntity = g_Interfaces.ServerGameEntsInstance->EdictToBaseEntity(edict);

	if(NULL == baseEntity) {
		throw InvalidEntityExceptionType(entityIndex);
	}

	if (offset <= 0 || offset > 32768) {
		throw EntityOffsetOutOfRangeExceptionType(offset);
	}

	return *(char *)((uint8_t *)baseEntity + offset);
}

unsigned char entity__get_entity_unsigned_char(int entityIndex, int offset) {
	edict_t *edict = PEntityOfEntIndex(entityIndex);

	if(edict == NULL || edict->IsFree()) {
		throw InvalidEdictExceptionType(entityIndex);
	}
	
	CBaseEntity *baseEntity = g_Interfaces.ServerGameEntsInstance->EdictToBaseEntity(edict);

	if(NULL == baseEntity) {
		throw InvalidEntityExceptionType(entityIndex);
	}

	if (offset <= 0 || offset > 32768) {
		throw EntityOffsetOutOfRangeExceptionType(offset);
	}

	return *(unsigned char *)((uint8_t *)baseEntity + offset);
}

float entity__get_entity_float(int entityIndex, int offset) {
	edict_t *edict = PEntityOfEntIndex(entityIndex);

	if(edict == NULL || edict->IsFree()) {
		throw InvalidEdictExceptionType(entityIndex);
	}
	
	CBaseEntity *baseEntity = g_Interfaces.ServerGameEntsInstance->EdictToBaseEntity(edict);

	if(NULL == baseEntity) {
		throw InvalidEntityExceptionType(entityIndex);
	}

	if (offset <= 0 || offset > 32768) {
		throw EntityOffsetOutOfRangeExceptionType(offset);
	}

	return *(float *)((uint8_t *)baseEntity + offset);
}

int entity__get_entity_entity(int entityIndex, int offset) {
	edict_t *edict = PEntityOfEntIndex(entityIndex);

	if(edict == NULL || edict->IsFree()) {
		throw InvalidEdictExceptionType(entityIndex);
	}
	
	CBaseEntity *baseEntity = g_Interfaces.ServerGameEntsInstance->EdictToBaseEntity(edict);

	if(NULL == baseEntity) {
		throw InvalidEntityExceptionType(entityIndex);
	}

	if (offset <= 0 || offset > 32768) {
		throw EntityOffsetOutOfRangeExceptionType(offset);
	}

	CBaseHandle &baseHandle = *(CBaseHandle *)((uint8_t *)baseEntity + offset);

	return baseHandle.GetEntryIndex();
}

VectorType entity__get_entity_vector(int entityIndex, int offset) {
	edict_t *edict = PEntityOfEntIndex(entityIndex);

	if(edict == NULL || edict->IsFree()) {
		throw InvalidEdictExceptionType(entityIndex);
	}
	
	CBaseEntity *baseEntity = g_Interfaces.ServerGameEntsInstance->EdictToBaseEntity(edict);

	if(NULL == baseEntity) {
		throw InvalidEntityExceptionType(entityIndex);
	}

	if (offset <= 0 || offset > 32768) {
		throw EntityOffsetOutOfRangeExceptionType(offset);
	}

	Vector *v = (Vector *)((uint8_t *)baseEntity + offset);

	return VectorType(v->x, v->y, v->z);
}

std::string entity__get_entity_string(int entityIndex, int offset) {
	edict_t *edict = PEntityOfEntIndex(entityIndex);

	if(edict == NULL || edict->IsFree()) {
		throw InvalidEdictExceptionType(entityIndex);
	}
	
	CBaseEntity *baseEntity = g_Interfaces.ServerGameEntsInstance->EdictToBaseEntity(edict);

	if(NULL == baseEntity) {
		throw InvalidEntityExceptionType(entityIndex);
	}

	if (offset <= 0 || offset > 32768) {
		throw EntityOffsetOutOfRangeExceptionType(offset);
	}

	char *str = (char *)((uint8_t *)baseEntity + offset);

	return std::string(str);
}

int entity__set_entity_int(int entityIndex, int offset, int newValue, bool changeState = false) {
	edict_t *edict = PEntityOfEntIndex(entityIndex);

	if(edict == NULL || edict->IsFree()) {
		throw InvalidEdictExceptionType(entityIndex);
	}
	
	CBaseEntity *baseEntity = g_Interfaces.ServerGameEntsInstance->EdictToBaseEntity(edict);

	if(NULL == baseEntity) {
		throw InvalidEntityExceptionType(entityIndex);
	}

	if (offset <= 0 || offset > 32768) {
		throw EntityOffsetOutOfRangeExceptionType(offset);
	}

	int oldValue = *(int *)((uint8_t *)baseEntity + offset);

	*(int *)((uint8_t *)baseEntity + offset) = newValue;

	if(changeState) {
		entity__change_edict_state_(edict, offset);
	}

	return oldValue;
}

unsigned int entity__set_entity_unsigned_int(int entityIndex, int offset, unsigned int newValue, bool changeState = false) {
	edict_t *edict = PEntityOfEntIndex(entityIndex);

	if(edict == NULL || edict->IsFree()) {
		throw InvalidEdictExceptionType(entityIndex);
	}
	
	CBaseEntity *baseEntity = g_Interfaces.ServerGameEntsInstance->EdictToBaseEntity(edict);

	if(NULL == baseEntity) {
		throw InvalidEntityExceptionType(entityIndex);
	}

	if (offset <= 0 || offset > 32768) {
		throw EntityOffsetOutOfRangeExceptionType(offset);
	}

	unsigned int oldValue = *(unsigned int *)((uint8_t *)baseEntity + offset);

	*(unsigned int *)((uint8_t *)baseEntity + offset) = newValue;

	if(changeState) {
		entity__change_edict_state_(edict, offset);
	}

	return oldValue;
}

short entity__set_entity_short(int entityIndex, int offset, short newValue, bool changeState = false) {
	edict_t *edict = PEntityOfEntIndex(entityIndex);

	if(edict == NULL || edict->IsFree()) {
		throw InvalidEdictExceptionType(entityIndex);
	}
	
	CBaseEntity *baseEntity = g_Interfaces.ServerGameEntsInstance->EdictToBaseEntity(edict);

	if(NULL == baseEntity) {
		throw InvalidEntityExceptionType(entityIndex);
	}

	if (offset <= 0 || offset > 32768) {
		throw EntityOffsetOutOfRangeExceptionType(offset);
	}

	short oldValue = *(short *)((uint8_t *)baseEntity + offset);

	*(short *)((uint8_t *)baseEntity + offset) = newValue;

	if(changeState) {
		entity__change_edict_state_(edict, offset);
	}

	return oldValue;
}

unsigned short entity__set_entity_unsigned_short(int entityIndex, int offset, unsigned short newValue, bool changeState = false) {
	edict_t *edict = PEntityOfEntIndex(entityIndex);

	if(edict == NULL || edict->IsFree()) {
		throw InvalidEdictExceptionType(entityIndex);
	}
	
	CBaseEntity *baseEntity = g_Interfaces.ServerGameEntsInstance->EdictToBaseEntity(edict);

	if(NULL == baseEntity) {
		throw InvalidEntityExceptionType(entityIndex);
	}

	if (offset <= 0 || offset > 32768) {
		throw EntityOffsetOutOfRangeExceptionType(offset);
	}

	unsigned short oldValue = *(unsigned short *)((uint8_t *)baseEntity + offset);

	*(unsigned short *)((uint8_t *)baseEntity + offset) = newValue;

	if(changeState) {
		entity__change_edict_state_(edict, offset);
	}

	return oldValue;
}

char entity__set_entity_char(int entityIndex, int offset, char newValue, bool changeState = false) {
	edict_t *edict = PEntityOfEntIndex(entityIndex);

	if(edict == NULL || edict->IsFree()) {
		throw InvalidEdictExceptionType(entityIndex);
	}
	
	CBaseEntity *baseEntity = g_Interfaces.ServerGameEntsInstance->EdictToBaseEntity(edict);

	if(NULL == baseEntity) {
		throw InvalidEntityExceptionType(entityIndex);
	}

	if (offset <= 0 || offset > 32768) {
		throw EntityOffsetOutOfRangeExceptionType(offset);
	}

	char oldValue = *(char *)((uint8_t *)baseEntity + offset);

	*(char *)((uint8_t *)baseEntity + offset) = newValue;

	if(changeState) {
		entity__change_edict_state_(edict, offset);
	}

	return oldValue;
}

unsigned char entity__set_entity_unsigned_char(int entityIndex, int offset, unsigned char newValue, bool changeState = false) {
	edict_t *edict = PEntityOfEntIndex(entityIndex);

	if(edict == NULL || edict->IsFree()) {
		throw InvalidEdictExceptionType(entityIndex);
	}
	
	CBaseEntity *baseEntity = g_Interfaces.ServerGameEntsInstance->EdictToBaseEntity(edict);

	if(NULL == baseEntity) {
		throw InvalidEntityExceptionType(entityIndex);
	}

	if (offset <= 0 || offset > 32768) {
		throw EntityOffsetOutOfRangeExceptionType(offset);
	}

	unsigned char oldValue = *(unsigned char *)((uint8_t *)baseEntity + offset);

	*(unsigned char *)((uint8_t *)baseEntity + offset) = newValue;

	if(changeState) {
		entity__change_edict_state_(edict, offset);
	}

	return oldValue;
}

float entity__set_entity_float(int entityIndex, int offset, float newValue, bool changeState = false) {
	edict_t *edict = PEntityOfEntIndex(entityIndex);

	if(edict == NULL || edict->IsFree()) {
		throw InvalidEdictExceptionType(entityIndex);
	}
	
	CBaseEntity *baseEntity = g_Interfaces.ServerGameEntsInstance->EdictToBaseEntity(edict);

	if(NULL == baseEntity) {
		throw InvalidEntityExceptionType(entityIndex);
	}

	if (offset <= 0 || offset > 32768) {
		throw EntityOffsetOutOfRangeExceptionType(offset);
	}

	float oldValue = *(float *)((uint8_t *)baseEntity + offset);

	*(float *)((uint8_t *)baseEntity + offset) = newValue;

	if(changeState) {
		entity__change_edict_state_(edict, offset);
	}

	return oldValue;
}

int entity__set_entity_entity(int entityIndex, int offset, int newValue, bool changeState = false) {
	edict_t *edict = PEntityOfEntIndex(entityIndex);

	if(edict == NULL || edict->IsFree()) {
		throw InvalidEdictExceptionType(entityIndex);
	}
	
	CBaseEntity *baseEntity = g_Interfaces.ServerGameEntsInstance->EdictToBaseEntity(edict);

	if(NULL == baseEntity) {
		throw InvalidEntityExceptionType(entityIndex);
	}

	if (offset <= 0 || offset > 32768) {
		throw EntityOffsetOutOfRangeExceptionType(offset);
	}

	CBaseHandle &baseHandle = *(CBaseHandle *)((uint8_t *)baseEntity + offset);

	int oldValue = baseHandle.GetEntryIndex();

	if(newValue == INVALID_EHANDLE_INDEX) {
		baseHandle.Set(NULL);
	}
	else {
		edict_t *otherEdict = PEntityOfEntIndex(newValue);

		if(otherEdict == NULL || otherEdict->IsFree()) {
			throw InvalidEdictExceptionType(newValue);
		}
	
		CBaseEntity *otherBaseEntity = g_Interfaces.ServerGameEntsInstance->EdictToBaseEntity(otherEdict);

		if(NULL == otherBaseEntity) {
			throw InvalidEntityExceptionType(newValue);
		}

		IHandleEntity *handleEntity = (IHandleEntity *)otherBaseEntity;
		baseHandle.Set(handleEntity);
	}

	if(changeState) {
		entity__change_edict_state_(edict, offset);
	}

	return oldValue;
}

VectorType entity__set_entity_vector(int entityIndex, int offset, VectorType newValue, bool changeState = false) {
	edict_t *edict = PEntityOfEntIndex(entityIndex);

	if(edict == NULL || edict->IsFree()) {
		throw InvalidEdictExceptionType(entityIndex);
	}
	
	CBaseEntity *baseEntity = g_Interfaces.ServerGameEntsInstance->EdictToBaseEntity(edict);

	if(NULL == baseEntity) {
		throw InvalidEntityExceptionType(entityIndex);
	}

	if (offset <= 0 || offset > 32768) {
		throw EntityOffsetOutOfRangeExceptionType(offset);
	}

	Vector *v = (Vector *)((uint8_t *)baseEntity + offset);

	VectorType oldValue(v->x, v->y, v->z);

	v->x = newValue.X;
	v->y = newValue.Y;
	v->z = newValue.Z;

	if(changeState) {
		entity__change_edict_state_(edict, offset);
	}

	return oldValue;
}

std::string entity__set_entity_string(int entityIndex, int offset, std::string newValue, int maxLength, bool changeState = false) {
	edict_t *edict = PEntityOfEntIndex(entityIndex);

	if(edict == NULL || edict->IsFree()) {
		throw InvalidEdictExceptionType(entityIndex);
	}
	
	CBaseEntity *baseEntity = g_Interfaces.ServerGameEntsInstance->EdictToBaseEntity(edict);

	if(NULL == baseEntity) {
		throw InvalidEntityExceptionType(entityIndex);
	}

	if (offset <= 0 || offset > 32768) {
		throw EntityOffsetOutOfRangeExceptionType(offset);
	}

	char *str = (char *)((uint8_t *)baseEntity + offset);

	std::string oldValue(str);

	UTIL_Format(str, maxLength, "%s", newValue.c_str());

	if(changeState) {
		entity__change_edict_state_(edict, offset);
	}

	return oldValue;
}

DEFINE_CUSTOM_EXCEPTION_INIT(InvalidEdictExceptionType, Entity)
DEFINE_CUSTOM_EXCEPTION_INIT(InvalidEntityExceptionType, Entity)
DEFINE_CUSTOM_EXCEPTION_INIT(EntityNotNetworkableExceptionType, Entity)
DEFINE_CUSTOM_EXCEPTION_INIT(InvalidSendPropertyExceptionType, Entity)
DEFINE_CUSTOM_EXCEPTION_INIT(InvalidDataMapExceptionType, Entity)
DEFINE_CUSTOM_EXCEPTION_INIT(InvalidDataMapPropertyExceptionType, Entity)
DEFINE_CUSTOM_EXCEPTION_INIT(EntityOffsetOutOfRangeExceptionType, Entity)

#if SOURCE_ENGINE != SE_DARKMESSIAH
IChangeInfoAccessor *CBaseEdict::GetChangeAccessor()
{
	return engine->GetChangeAccessor( (const edict_t *)this );
}
#endif

BOOST_PYTHON_MODULE(Entity) {
	py::enum_<EntityPropertyFieldTypes>("EntityPropertyFieldType")
		.value("Unsupported", PropField_Unsupported)
		.value("Integer", PropField_Integer)
		.value("Float", PropField_Float)
		.value("Entity", PropField_Entity)
		.value("Vector", PropField_Vector)
		.value("String", PropField_String)
		.value("String_T", PropField_String_T);

	py::class_<EntitySendPropertyInfoType>("EntitySendPropertyInfo", py::no_init)
		.def_readonly("PropertyFieldType", &EntitySendPropertyInfoType::PropertyFieldType)
		.def_readonly("NumBits", &EntitySendPropertyInfoType::NumBits)
		.def_readonly("LocalOffset", &EntitySendPropertyInfoType::LocalOffset)
		.def_readonly("AbsoluteOffset", &EntitySendPropertyInfoType::AbsoluteOffset);

	py::class_<EntityDataMapInfoType>("EntityDataMapInfo", py::no_init)
		.def_readonly("PropertyFieldType", &EntityDataMapInfoType::PropertyFieldType)
		.def_readonly("NumBits", &EntityDataMapInfoType::NumBits)
		.def_readonly("Offset", &EntityDataMapInfoType::Offset);

	py::def("GetMaxEntities", &entity__get_max_entities);
	py::def("GetEntityCount", &entity__get_entity_count);
	py::def("IsValidEntity", &entity__is_valid_entity, (py::arg("entity_index")));
	py::def("IsValidEdict", &entity__is_valid_edict, (py::arg("edict_index")));
	py::def("IsEntityNetworkable", &entity__is_entity_networkable, (py::arg("entity_index")));
	py::def("CreateEdict", &entity__create_edict);
	py::def("RemoveEdict", &entity__remove_edict, (py::arg("edict_index")));
	py::def("GetEdictFlags", &entity__get_edict_flags, (py::arg("edict_index")));
	py::def("SetEdictFlags", &entity__set_edict_flags, (py::arg("edict_index"), py::arg("edict_flags")));
	py::def("GetEdictClassName", &entity__get_edict_class_name, (py::arg("edict_index")));
	py::def("GetEntityNetClass", &entity__get_entity_net_class, (py::arg("entity_index")));
	py::def("ChangeEdictState", &entity__change_edict_state, (py::arg("edict_index"), py::arg("offset") = 0));
	py::def("FindSendPropertyOffset", &entity__find_send_property_offset, (py::arg("class_name"), py::arg("property_name")));
	py::def("FindSendPropertyInfo", &entity__find_send_property_info, (py::arg("class_name"), py::arg("property_name")));
	py::def("FindDataMapInfo", &entity__find_data_map_info, (py::arg("entity_index"), py::arg("property_name")));
	py::def("GetEntityInt", &entity__get_entity_int, (py::arg("entity_index"), py::arg("offset")));
	py::def("GetEntityUnsignedInt", &entity__get_entity_unsigned_int, (py::arg("entity_index"), py::arg("offset")));
	py::def("GetEntityShort", &entity__get_entity_short, (py::arg("entity_index"), py::arg("offset")));
	py::def("GetEntityUnsignedShort", &entity__get_entity_unsigned_short, (py::arg("entity_index"), py::arg("offset")));
	py::def("GetEntityChar", &entity__get_entity_char, (py::arg("entity_index"), py::arg("offset")));
	py::def("GetEntityUnsignedChar", &entity__get_entity_unsigned_char, (py::arg("entity_index"), py::arg("offset")));
	py::def("GetEntityFloat", &entity__get_entity_float, (py::arg("entity_index"), py::arg("offset")));
	py::def("GetEntityEntity", &entity__get_entity_entity, (py::arg("entity_index"), py::arg("offset")));
	py::def("GetEntityVector", &entity__get_entity_vector, (py::arg("entity_index"), py::arg("offset")));
	py::def("GetEntityString", &entity__get_entity_string, (py::arg("entity_index"), py::arg("offset")));
	py::def("SetEntityInt", &entity__set_entity_int, (py::arg("entity_index"), py::arg("offset"), py::arg("new_value"), py::arg("change_state") = false));
	py::def("SetEntityUnsignedInt", &entity__set_entity_unsigned_int, (py::arg("entity_index"), py::arg("offset"), py::arg("new_value"), py::arg("change_state") = false));
	py::def("SetEntityShort", &entity__set_entity_short, (py::arg("entity_index"), py::arg("offset"), py::arg("new_value"), py::arg("change_state") = false));
	py::def("SetEntityUnsignedShort", &entity__set_entity_unsigned_short, (py::arg("entity_index"), py::arg("offset"), py::arg("new_value"), py::arg("change_state") = false));
	py::def("SetEntityChar", &entity__set_entity_char, (py::arg("entity_index"), py::arg("offset"), py::arg("new_value"), py::arg("change_state") = false));
	py::def("SetEntityUnsignedChar", &entity__set_entity_unsigned_char, (py::arg("entity_index"), py::arg("offset"), py::arg("new_value"), py::arg("change_state") = false));
	py::def("SetEntityFloat", &entity__set_entity_float, (py::arg("entity_index"), py::arg("offset"), py::arg("new_value"), py::arg("change_state") = false));
	py::def("SetEntityEntity", &entity__set_entity_entity, (py::arg("entity_index"), py::arg("offset"), py::arg("new_value"), py::arg("change_state") = false));
	py::def("SetEntityVector", &entity__set_entity_vector, (py::arg("entity_index"), py::arg("offset"), py::arg("new_value"), py::arg("change_state") = false));
	py::def("SetEntityString", &entity__set_entity_string, (py::arg("entity_index"), py::arg("offset"), py::arg("new_value"), py::arg("max_length"), py::arg("change_state") = false));

	DEFINE_CUSTOM_EXCEPTION(InvalidEdictExceptionType, Entity,
		PyExc_Exception, "Entity.InvalidEdictException",
		"InvalidEdictException")

	DEFINE_CUSTOM_EXCEPTION(InvalidEntityExceptionType, Entity,
		PyExc_Exception, "Entity.InvalidEntityException",
		"InvalidEntityException")

	DEFINE_CUSTOM_EXCEPTION(EntityNotNetworkableExceptionType, Entity,
		PyExc_Exception, "Entity.EntityNotNetworkableException",
		"EntityNotNetworkableException")

	DEFINE_CUSTOM_EXCEPTION(InvalidSendPropertyExceptionType, Entity,
		PyExc_Exception, "Entity.InvalidSendPropertyException",
		"InvalidSendPropertyException")

	DEFINE_CUSTOM_EXCEPTION(InvalidDataMapExceptionType, Entity,
		PyExc_Exception, "Entity.InvalidDataMapException",
		"InvalidDataMapException")

	DEFINE_CUSTOM_EXCEPTION(InvalidDataMapPropertyExceptionType, Entity,
		PyExc_Exception, "Entity.InvalidDataMapPropertyException",
		"InvalidDataMapPropertyException")

	DEFINE_CUSTOM_EXCEPTION(EntityOffsetOutOfRangeExceptionType, Entity,
		PyExc_Exception, "Entity.EntityOffsetOutOfRangeException",
		"EntityOffsetOutOfRangeException")
}

void destroyEntity() {
}

void unloadThreadStateEntity(PyThreadState *threadState) {
}