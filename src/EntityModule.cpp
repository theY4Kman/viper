#include "BoostPythonSM.h"
#include "EntityModule.h"
#include "sdk/smsdk_ext.h"
#include "Macros.h"
#include "ViperExtension.h"
#include "EdictType.h"
#include "InvalidEdictExceptionType.h"

namespace py = boost::python;

int entity__get_max_entities() {
	return g_Interfaces.GlobalVarsInstance->maxEntities;
}

int entity__get_entity_count() {
	return engine->GetEntityCount();
}

bool entity__is_valid_entity(int entityIndex) {
	edict_t *edict = engine->PEntityOfEntIndex(entityIndex);

	if(edict == NULL || edict->IsFree()) {
		return false;
	}

	IServerUnknown *unknown = edict->GetUnknown();

	if(unknown == NULL) {
		return false;
	}

	CBaseEntity *baseEntity = unknown->GetBaseEntity();

	return baseEntity != NULL;
}

bool entity__is_valid_edict(int entityIndex) {
	edict_t *edict = engine->PEntityOfEntIndex(entityIndex);

	if(edict == NULL) {
		return false;
	}

	return !edict->IsFree();
}

bool entity__is_entity_networkable(int entityIndex) {
	edict_t *edict = engine->PEntityOfEntIndex(entityIndex);

	if(edict == NULL || edict->IsFree()) {
		return false;
	}

	IServerNetworkable *networkable = edict->GetNetworkable();

	return networkable != NULL;
}

EdictType entity__create_edict() {
	edict_t *edict = engine->CreateEdict();

	if(edict == NULL || edict->IsFree()) {
		throw InvalidEdictExceptionType(edict);
	}

	return EdictType(edict);
}

BOOST_PYTHON_MODULE(entity) {
	py::class_<EdictType>("Edict", py::no_init)
		.def("is_valid", &EdictType::IsValid)
		.def("__str__", &EdictType::StrMagic);

	py::def("get_max_entities", entity__get_max_entities);
	py::def("get_entity_count", entity__get_entity_count);
	py::def("is_entity_networkable", entity__is_entity_networkable);
	py::def("create_edict", entity__create_edict);
}