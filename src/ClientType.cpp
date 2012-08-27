#include "ClientType.h"
#include "Macros.h"
#include "BoostPythonSM.h"
#include "Util.h"
#include "InvalidEdictExceptionType.h"
#include "InvalidEntityExceptionType.h"
#include "CompatWrappers.h"
#include "InterfaceContainer.h"
#include "ClientIndexOutOfRangeExceptionType.h"

namespace py = boost::python;

ClientType::ClientType(int index) {
	if(index <= 0 || index > g_Interfaces.GlobalVarsInstance->maxClients) {
		throw ClientIndexOutOfRangeExceptionType(index, g_Interfaces.GlobalVarsInstance->maxClients);
	}

	Index = index;
}

EntityType ClientType::GetEntity() {
	edict_t *edict = PEntityOfEntIndex(Index);

	if(edict == NULL || edict->IsFree()) {
		throw InvalidEdictExceptionType(edict);
	}

	CBaseEntity *entity = g_Interfaces.ServerGameEntsInstance->EdictToBaseEntity(edict);

	if(entity == NULL) {
		throw InvalidEntityExceptionType();
	}

	return EntityType(entity);
}

EdictType ClientType::GetEdict() {
	edict_t *edict = PEntityOfEntIndex(Index);

	if(edict == NULL || edict->IsFree()) {
		throw InvalidEdictExceptionType(edict);
	}

	return EdictType(edict);
}

std::string ClientType::GetName() {
	return "";
}

bool ClientType::IsConnected() {
	return false;
}

bool ClientType::IsInGame() {
	return false;
}

std::string ClientType::StrMagic() {
	char str[256];
	UTIL_Format(str, sizeof(str), "<Client object at: %p>", this);

	return std::string(str);
}