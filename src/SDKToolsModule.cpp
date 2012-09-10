/**
 * Fair warning: Like ALOT of Viper, SourceMod helpers + SDKTools code is reused! Thanks to the entire SM team!
 */

#include "SDKToolsModule.h"
#include "InterfaceContainer.h"
#include "ClientNotConnectedExceptionType.h"
#include "ClientDataNotAvailableExceptionType.h"
#include "IServerNotFoundExceptionType.h"
#include "ClientNotInGameExceptionType.h"
#include "InvalidEdictExceptionType.h"
#include "InvalidEntityExceptionType.h"
#include "LightStyleOutOfRangeExceptionType.h"
#include "SDKToolsModSupportNotAvailableExceptionType.h"
#include "VectorType.h"
#include <public\iclient.h>
#include "Util.h"

namespace py = boost::python;

SourceMod::ICallWrapper *sdktools__AcceptEntityInputCallWrapper = NULL;
unsigned char sdktools__VariantTInstance[SIZEOF_VARIANT_T] = {0};

void sdktools__init_variant_t() {
	unsigned char *vptr = sdktools__VariantTInstance;

	*(int *)vptr = 0;
	vptr += sizeof(int)*3;
	*(unsigned long *)vptr = INVALID_EHANDLE_INDEX;
	vptr += sizeof(unsigned long);
	*(fieldtype_t *)vptr = FIELD_VOID;
}

void sdktools__inactive_client(int clientIndex) {
	SourceMod::IGamePlayer *player = playerhelpers->GetGamePlayer(clientIndex);
	
	if(!player->IsConnected()) {
		throw ClientNotConnectedExceptionType(clientIndex);
	}

	IServer *server = g_Interfaces.SDKToolsInstance->GetIServer();

	if (NULL == server) {
		throw IServerNotFoundExceptionType();
	}

	IClient *client = server->GetClient(clientIndex);
	
	if(NULL == client) {
		throw ClientDataNotAvailableExceptionType(clientIndex, "iclient");
	}

	client->Inactivate();
}

void sdktools__reconnect_client(int clientIndex) {
	SourceMod::IGamePlayer *player = playerhelpers->GetGamePlayer(clientIndex);
	
	if(!player->IsConnected()) {
		throw ClientNotConnectedExceptionType(clientIndex);
	}

	IServer *server = g_Interfaces.SDKToolsInstance->GetIServer();

	if (NULL == server) {
		throw IServerNotFoundExceptionType();
	}

	IClient *client = server->GetClient(clientIndex);
	
	if(NULL == client) {
		throw ClientDataNotAvailableExceptionType(clientIndex, "iclient");
	}

	client->Reconnect();
}

void sdktools__set_client_view_entity(int clientIndex, int entityIndex) {
	SourceMod::IGamePlayer *player = playerhelpers->GetGamePlayer(clientIndex);

	if(!player->IsConnected()) {
		throw ClientNotConnectedExceptionType(clientIndex);
	}

	if(!player->IsInGame()) {
		throw ClientNotInGameExceptionType(clientIndex);
	}

	edict_t *edict = PEntityOfEntIndex(entityIndex);
	
	if(!edict || edict->IsFree()) {
		throw InvalidEdictExceptionType(entityIndex);
	}

	engine->SetView(player->GetEdict(), edict);
}

void sdktools__set_light_style(int style, std::string lightStr) {
	if(style < 0 || style >= MAX_LIGHTSTYLES) {
		throw LightStyleOutOfRangeExceptionType(style);
	}

	char *str = UTIL_Strdup(lightStr.c_str());

	engine->LightStyle(style, str);
}

VectorType sdktools__get_client_eye_position(int clientIndex) {
	SourceMod::IGamePlayer *player = playerhelpers->GetGamePlayer(clientIndex);

	if(!player->IsConnected()) {
		throw ClientNotConnectedExceptionType(clientIndex);
	}

	if(!player->IsInGame()) {
		throw ClientNotInGameExceptionType(clientIndex);
	}

	Vector p;
	g_Interfaces.ServerGameClientsInstance->ClientEarPosition(player->GetEdict(), &p);

	return VectorType(p.x, p.y, p.z);
}

bool sdktools__accept_entity_input(int entityIndex, std::string action, int activatorIndex = -1, int callerIndex = -1, int outputID = 0) {
	if (NULL == sdktools__AcceptEntityInputCallWrapper) {
		int offset;

		if (!g_Interfaces.GameConfigInstance->GetOffset("AcceptInput", &offset)) {
			throw SDKToolsModSupportNotAvailableExceptionType("AcceptEntityInput");
		}

		PassInfo pass[6];
		pass[0].type = PassType_Basic;
		pass[0].flags = PASSFLAG_BYVAL;
		pass[0].size = sizeof(const char *);
		pass[1].type = pass[2].type = PassType_Basic;
		pass[1].flags = pass[2].flags = PASSFLAG_BYVAL;
		pass[1].size = pass[2].size = sizeof(CBaseEntity *);
		pass[3].type = PassType_Object;
		pass[3].flags = PASSFLAG_BYVAL|PASSFLAG_OCTOR|PASSFLAG_ODTOR|PASSFLAG_OASSIGNOP;
		pass[3].size = SIZEOF_VARIANT_T;
		pass[4].type = PassType_Basic;
		pass[4].flags = PASSFLAG_BYVAL;
		pass[4].size = sizeof(int);
		pass[5].type = PassType_Basic;
		pass[5].flags = PASSFLAG_BYVAL;
		pass[5].size = sizeof(bool);

		if (!(sdktools__AcceptEntityInputCallWrapper = g_Interfaces.BinToolsInstance->CreateVCall(offset, 0, 0, &pass[5], pass, 5))) {
			throw SDKToolsModSupportNotAvailableExceptionType("AcceptEntityInput");
		}
	}

	CBaseEntity *activatorEntity, *callerEntity, *entity;

	char *inputname;
	
	unsigned char vstk[sizeof(void *) + sizeof(const char *) + sizeof(CBaseEntity *)*2 + SIZEOF_VARIANT_T + sizeof(int)];
	unsigned char *vptr = vstk;

	edict_t *edict = PEntityOfEntIndex(entityIndex);

	if(edict == NULL || edict->IsFree()) {
		throw InvalidEdictExceptionType(entityIndex);
	}

	entity = g_Interfaces.ServerGameEntsInstance->EdictToBaseEntity(edict);

	if(entity == NULL) {
		throw InvalidEntityExceptionType(entityIndex);
	}

	if(activatorIndex == -1) {
		activatorEntity = NULL;
	} else {
		edict_t *activatorEdict = PEntityOfEntIndex(activatorIndex);

		if(activatorEdict == NULL || activatorEdict->IsFree()) {
			throw InvalidEdictExceptionType(activatorIndex);
		}

		activatorEntity = g_Interfaces.ServerGameEntsInstance->EdictToBaseEntity(activatorEdict);

		if(activatorEntity == NULL) {
			throw InvalidEntityExceptionType(activatorIndex);
		}
	}

	if (callerIndex == -1) {
		callerEntity = NULL;
	} else {
		edict_t *callerEdict = PEntityOfEntIndex(callerIndex);

		if(callerEdict == NULL || callerEdict->IsFree()) {
			throw InvalidEdictExceptionType(callerIndex);
		}

		callerEntity = g_Interfaces.ServerGameEntsInstance->EdictToBaseEntity(callerEdict);

		if(callerEntity == NULL) {
			throw InvalidEntityExceptionType(callerIndex);
		}
	}

	*(void **)vptr = entity;
	vptr += sizeof(void *);
	*(const char **)vptr = action.c_str();
	vptr += sizeof(const char *);
	*(CBaseEntity **)vptr = activatorEntity;
	vptr += sizeof(CBaseEntity *);
	*(CBaseEntity **)vptr = callerEntity;
	vptr += sizeof(CBaseEntity *);
	memcpy(vptr, sdktools__VariantTInstance, SIZEOF_VARIANT_T);
	vptr += SIZEOF_VARIANT_T;
	*(int *)vptr = outputID;

	bool ret;
	sdktools__AcceptEntityInputCallWrapper->Execute(vstk, &ret);

	sdktools__init_variant_t();

	return ret;
}

DEFINE_CUSTOM_EXCEPTION_INIT(IServerNotFoundExceptionType, SDKTools)
DEFINE_CUSTOM_EXCEPTION_INIT(LightStyleOutOfRangeExceptionType, SDKTools)
DEFINE_CUSTOM_EXCEPTION_INIT(SDKToolsModSupportNotAvailableExceptionType, SDKTools)

BOOST_PYTHON_MODULE(SDKTools) {
	py::def("InactivateClient", &sdktools__inactive_client, (py::arg("client_index")));
	py::def("ReconnectClient", &sdktools__reconnect_client, (py::arg("client_index")));
	py::def("SetClientViewEntity", &sdktools__set_client_view_entity, (py::arg("client_index"), py::arg("entity_index")));
	py::def("SetLightStyle", &sdktools__set_light_style, (py::arg("style"), py::arg("light_str")));
	py::def("GetClientEyePosition", &sdktools__get_client_eye_position, (py::arg("client_index")));
	py::def("AcceptEntityInput", &sdktools__accept_entity_input, (py::arg("entity_index"), py::arg("action"), py::arg("activator") = -1, py::arg("caller") = -1, py::arg("output_id") = 0));

	DEFINE_CUSTOM_EXCEPTION(IServerNotFoundExceptionType, SDKTools,
		PyExc_Exception, "SDKTools.IServerNotFoundException",
		"IServerNotFoundException")

	DEFINE_CUSTOM_EXCEPTION(LightStyleOutOfRangeExceptionType, SDKTools,
		PyExc_Exception, "SDKTools.LightStyleOutOfRangeException",
		"LightStyleOutOfRangeException")

	DEFINE_CUSTOM_EXCEPTION(SDKToolsModSupportNotAvailableExceptionType, SDKTools,
		PyExc_Exception, "SDKTools.SDKToolsModSupportNotAvailableException",
		"SDKToolsModSupportNotAvailableException")
}

void destroySDKTools() {

}

void unloadThreadStateSDKTools(PyThreadState *threadState) {
}