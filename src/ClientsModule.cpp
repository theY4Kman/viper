#include "BoostPythonSM.h"
#include "ClientsModule.h"
#include "InterfaceContainer.h"
#include "ClientDataNotAvailableExceptionType.h"
#include "ClientIndexOutOfRangeExceptionType.h"
#include "ClientNotConnectedExceptionType.h"
#include <boost/regex.hpp>
#include "game/server/iplayerinfo.h"
#include "CompatWrappers.h"
#include "InvalidEdictExceptionType.h"
#include "ClientNotInGameExceptionType.h"
#include "ClientNotFakeExceptionType.h"
#include "ClientIsFakeExceptionType.h"
#include "InvalidUserIdExceptionType.h"
#include "InvalidClientSerialExceptionType.h"
#include "ClientNotAuthorizedExceptionType.h"
#include "VectorType.h"
#include "inetchannelinfo.h"
#include "server_class.h"
#include "dt_common.h"

namespace py = boost::python;

std::map<int, std::string> clients__KickQueue;

int clients__LifeStateOffset = -1;

int clients__get_max_clients() {
	return g_Interfaces.GlobalVarsInstance->maxClients;
}

int clients__get_client_count(bool inGameOnly = true) {
	int count = 0;

	for (int i = 1; i <= g_Interfaces.GlobalVarsInstance->maxClients; i++) {
		SourceMod::IGamePlayer *gamePlayer = playerhelpers->GetGamePlayer(i);

		if(!gamePlayer->IsConnected()) {
			continue;
		}

		if(inGameOnly && !gamePlayer->IsInGame()) {
			continue;
		}

		count++;
	}

	return count;
}

std::string clients__get_client_name(int clientIndex) {
	if(clientIndex < 1 || clientIndex > g_Interfaces.GlobalVarsInstance->maxClients) {
		throw ClientIndexOutOfRangeExceptionType(clientIndex, g_Interfaces.GlobalVarsInstance->maxClients);
	}

	SourceMod::IGamePlayer *gamePlayer = playerhelpers->GetGamePlayer(clientIndex);

	if(!gamePlayer->IsConnected()) {
		throw ClientNotConnectedExceptionType(clientIndex);
	}

	const char *playerName = gamePlayer->GetName();

	if(NULL == playerName) {
		throw ClientDataNotAvailableExceptionType(clientIndex, "name");
	}

	return std::string(playerName);
}

std::string clients__get_client_ip(int clientIndex, bool removePort = true) {
	if(clientIndex < 1 || clientIndex > g_Interfaces.GlobalVarsInstance->maxClients) {
		throw ClientIndexOutOfRangeExceptionType(clientIndex, g_Interfaces.GlobalVarsInstance->maxClients);
	}

	SourceMod::IGamePlayer *gamePlayer = playerhelpers->GetGamePlayer(clientIndex);

	if(!gamePlayer->IsConnected()) {
		throw ClientNotConnectedExceptionType(clientIndex);
	}

	const char *ipAddress  = gamePlayer->GetIPAddress();

	if(NULL == ipAddress) {
		throw ClientDataNotAvailableExceptionType(clientIndex, "ip_address");
	}

	std::string ipAddressStr(ipAddress);

	if(removePort) {
		auto colonIndex = ipAddressStr.find(':');

		if(colonIndex != std::string::npos) {
			ipAddressStr = ipAddressStr.substr(0, colonIndex);
		}
	}

	return ipAddressStr;
}

std::string clients__get_client_auth_string(int clientIndex) {
	if(clientIndex < 1 || clientIndex > g_Interfaces.GlobalVarsInstance->maxClients) {
		throw ClientIndexOutOfRangeExceptionType(clientIndex, g_Interfaces.GlobalVarsInstance->maxClients);
	}

	SourceMod::IGamePlayer *gamePlayer = playerhelpers->GetGamePlayer(clientIndex);

	if(!gamePlayer->IsConnected()) {
		throw ClientNotConnectedExceptionType(clientIndex);
	}

	if(!gamePlayer->IsAuthorized()) {
		throw ClientNotAuthorizedExceptionType(clientIndex);
	}

	const char *authString = gamePlayer->GetAuthString();

	if(NULL == authString) {
		throw ClientDataNotAvailableExceptionType(clientIndex, "auth_string");
	}

	return std::string(authString);
}

int clients__get_client_user_id(int clientIndex) {
	if(clientIndex < 1 || clientIndex > g_Interfaces.GlobalVarsInstance->maxClients) {
		throw ClientIndexOutOfRangeExceptionType(clientIndex, g_Interfaces.GlobalVarsInstance->maxClients);
	}

	SourceMod::IGamePlayer *gamePlayer = playerhelpers->GetGamePlayer(clientIndex);

	if(!gamePlayer->IsConnected()) {
		throw ClientNotConnectedExceptionType(clientIndex);
	}

	int userId = gamePlayer->GetUserId();

	return userId;
}

bool clients__is_client_connected(int clientIndex) {
	if(clientIndex < 1 || clientIndex > g_Interfaces.GlobalVarsInstance->maxClients) {
		throw ClientIndexOutOfRangeExceptionType(clientIndex, g_Interfaces.GlobalVarsInstance->maxClients);
	}

	SourceMod::IGamePlayer *gamePlayer = playerhelpers->GetGamePlayer(clientIndex);

	return gamePlayer->IsConnected();
}

bool clients__is_client_in_game(int clientIndex) {
	if(clientIndex < 1 || clientIndex > g_Interfaces.GlobalVarsInstance->maxClients) {
		throw ClientIndexOutOfRangeExceptionType(clientIndex, g_Interfaces.GlobalVarsInstance->maxClients);
	}

	SourceMod::IGamePlayer *gamePlayer = playerhelpers->GetGamePlayer(clientIndex);

	if(!gamePlayer->IsConnected()) {
		throw ClientNotConnectedExceptionType(clientIndex);
	}

	return gamePlayer->IsInGame();
}

bool clients__is_client_in_kick_queue(int clientIndex) {
	if(clientIndex < 1 || clientIndex > g_Interfaces.GlobalVarsInstance->maxClients) {
		throw ClientIndexOutOfRangeExceptionType(clientIndex, g_Interfaces.GlobalVarsInstance->maxClients);
	}

	SourceMod::IGamePlayer *gamePlayer = playerhelpers->GetGamePlayer(clientIndex);

	if(!gamePlayer->IsConnected()) {
		throw ClientNotConnectedExceptionType(clientIndex);
	}

	return gamePlayer->IsInKickQueue();
}

bool clients__is_client_authorized(int clientIndex) {
	if(clientIndex < 1 || clientIndex > g_Interfaces.GlobalVarsInstance->maxClients) {
		throw ClientIndexOutOfRangeExceptionType(clientIndex, g_Interfaces.GlobalVarsInstance->maxClients);
	}

	SourceMod::IGamePlayer *gamePlayer = playerhelpers->GetGamePlayer(clientIndex);

	if(!gamePlayer->IsConnected()) {
		throw ClientNotConnectedExceptionType(clientIndex);
	}

	return gamePlayer->IsAuthorized();
}

bool clients__is_client_fake(int clientIndex) {
	if(clientIndex < 1 || clientIndex > g_Interfaces.GlobalVarsInstance->maxClients) {
		throw ClientIndexOutOfRangeExceptionType(clientIndex, g_Interfaces.GlobalVarsInstance->maxClients);
	}

	SourceMod::IGamePlayer *gamePlayer = playerhelpers->GetGamePlayer(clientIndex);

	if(!gamePlayer->IsConnected()) {
		throw ClientNotConnectedExceptionType(clientIndex);
	}

	return gamePlayer->IsFakeClient();
}

bool clients__is_client_source_tv(int clientIndex) {
	if(clientIndex < 1 || clientIndex > g_Interfaces.GlobalVarsInstance->maxClients) {
		throw ClientIndexOutOfRangeExceptionType(clientIndex, g_Interfaces.GlobalVarsInstance->maxClients);
	}

	SourceMod::IGamePlayer *gamePlayer = playerhelpers->GetGamePlayer(clientIndex);

	if(!gamePlayer->IsConnected()) {
		throw ClientNotConnectedExceptionType(clientIndex);
	}

	return gamePlayer->IsSourceTV();
}

bool clients__is_client_replay(int clientIndex) {
	if(clientIndex < 1 || clientIndex > g_Interfaces.GlobalVarsInstance->maxClients) {
		throw ClientIndexOutOfRangeExceptionType(clientIndex, g_Interfaces.GlobalVarsInstance->maxClients);
	}

	SourceMod::IGamePlayer *gamePlayer = playerhelpers->GetGamePlayer(clientIndex);

	if(!gamePlayer->IsConnected()) {
		throw ClientNotConnectedExceptionType(clientIndex);
	}

	return gamePlayer->IsReplay();
}

bool clients__is_client_observer(int clientIndex) {
	if(clientIndex < 1 || clientIndex > g_Interfaces.GlobalVarsInstance->maxClients) {
		throw ClientIndexOutOfRangeExceptionType(clientIndex, g_Interfaces.GlobalVarsInstance->maxClients);
	}

	SourceMod::IGamePlayer *gamePlayer = playerhelpers->GetGamePlayer(clientIndex);

	if(!gamePlayer->IsConnected()) {
		throw ClientNotConnectedExceptionType(clientIndex); 
	}

	if(!gamePlayer->IsInGame()) {
		throw ClientNotInGameExceptionType(clientIndex);
	}
	
	IPlayerInfo *playerInfo = gamePlayer->GetPlayerInfo();

	if(NULL == playerInfo) {
		throw ClientDataNotAvailableExceptionType(clientIndex, "player_info"); 
	}

	return playerInfo->IsObserver();
}

bool clients__is_client_alive_(IGamePlayer *gamePlayer) {
	edict_t *edict = gamePlayer->GetEdict();

	if(edict == NULL || edict->IsFree()) {
		return false;
	}

	int clientIndex = IndexOfEdict(edict);

	CBaseEntity *baseEntity = g_Interfaces.ServerGameEntsInstance->EdictToBaseEntity(edict);

	if(NULL == baseEntity) {
		return false;
	}

	if(clients__LifeStateOffset == -1) {
		SendProp *lifeState = gamehelpers->FindInSendTable("CBasePlayer", "m_lifeState");

		if(NULL == lifeState) {
			throw ClientDataNotAvailableExceptionType(clientIndex, "life_state");
		}

		clients__LifeStateOffset = lifeState->GetOffset();
	}

	return *((uint8_t *)baseEntity + clients__LifeStateOffset) == LIFE_ALIVE;
}

bool clients__is_client_alive(int clientIndex) {
	if(clientIndex < 1 || clientIndex > g_Interfaces.GlobalVarsInstance->maxClients) {
		throw ClientIndexOutOfRangeExceptionType(clientIndex, g_Interfaces.GlobalVarsInstance->maxClients);
	}

	SourceMod::IGamePlayer *gamePlayer = playerhelpers->GetGamePlayer(clientIndex);

	if(!gamePlayer->IsConnected()) {
		throw ClientNotConnectedExceptionType(clientIndex);
	}

	return clients__is_client_alive_(gamePlayer);
}

std::string clients__get_client_info(int clientIndex, std::string key) {
	if(clientIndex < 1 || clientIndex > g_Interfaces.GlobalVarsInstance->maxClients) {
		throw ClientIndexOutOfRangeExceptionType(clientIndex, g_Interfaces.GlobalVarsInstance->maxClients);
	}

	SourceMod::IGamePlayer *gamePlayer = playerhelpers->GetGamePlayer(clientIndex);

	if(!gamePlayer->IsConnected()) {
		throw ClientNotConnectedExceptionType(clientIndex);
	}

	const char *userInfo = engine->GetClientConVarValue(clientIndex, key.c_str());

	if(NULL == userInfo) {
		throw ClientDataNotAvailableExceptionType(clientIndex, std::string("user_info[") + key + std::string("]")); 
	}

	return std::string(userInfo);
}

int clients__get_client_team(int clientIndex) {
	if(clientIndex < 1 || clientIndex > g_Interfaces.GlobalVarsInstance->maxClients) {
		throw ClientIndexOutOfRangeExceptionType(clientIndex, g_Interfaces.GlobalVarsInstance->maxClients);
	}

	SourceMod::IGamePlayer *gamePlayer = playerhelpers->GetGamePlayer(clientIndex);

	if(!gamePlayer->IsConnected()) {
		throw ClientNotConnectedExceptionType(clientIndex);
	}

	if(!gamePlayer->IsInGame()) {
		throw ClientNotInGameExceptionType(clientIndex);
	}

	IPlayerInfo *playerInfo = gamePlayer->GetPlayerInfo();

	if(NULL == playerInfo) {
		throw ClientDataNotAvailableExceptionType(clientIndex, "player_info"); 
	}

	return playerInfo->GetTeamIndex();
}

int clients__create_fake_client(std::string name) {
	edict_t *edict = engine->CreateFakeClient(name.c_str());

	if(edict == NULL || edict->IsFree()) {
		return -1;
	}

	return IndexOfEdict(edict);
}

void clients__set_fake_client_cvar(int clientIndex, std::string cvarName, std::string newValue) {
	if(clientIndex < 1 || clientIndex > g_Interfaces.GlobalVarsInstance->maxClients) {
		throw ClientIndexOutOfRangeExceptionType(clientIndex, g_Interfaces.GlobalVarsInstance->maxClients);
	}

	SourceMod::IGamePlayer *gamePlayer = playerhelpers->GetGamePlayer(clientIndex);

	if(!gamePlayer->IsConnected()) {
		throw ClientNotConnectedExceptionType(clientIndex);
	}

	if(!gamePlayer->IsFakeClient()) {
		throw ClientNotFakeExceptionType(clientIndex);
	}

	engine->SetFakeClientConVarValue(gamePlayer->GetEdict(), cvarName.c_str(), newValue.c_str());
}

int clients__get_client_health(int clientIndex) {
	if(clientIndex < 1 || clientIndex > g_Interfaces.GlobalVarsInstance->maxClients) {
		throw ClientIndexOutOfRangeExceptionType(clientIndex, g_Interfaces.GlobalVarsInstance->maxClients);
	}

	SourceMod::IGamePlayer *gamePlayer = playerhelpers->GetGamePlayer(clientIndex);

	if(!gamePlayer->IsConnected()) {
		throw ClientNotConnectedExceptionType(clientIndex);
	}

	if(!gamePlayer->IsInGame()) {
		throw ClientNotInGameExceptionType(clientIndex);
	}

	IPlayerInfo *playerInfo = gamePlayer->GetPlayerInfo();

	if(NULL == playerInfo) {
		throw ClientDataNotAvailableExceptionType(clientIndex, "player_info"); 
	}

	return playerInfo->GetHealth();
}

std::string clients__get_client_model(int clientIndex) {
	if(clientIndex < 1 || clientIndex > g_Interfaces.GlobalVarsInstance->maxClients) {
		throw ClientIndexOutOfRangeExceptionType(clientIndex, g_Interfaces.GlobalVarsInstance->maxClients);
	}

	SourceMod::IGamePlayer *gamePlayer = playerhelpers->GetGamePlayer(clientIndex);

	if(!gamePlayer->IsConnected()) {
		throw ClientNotConnectedExceptionType(clientIndex);
	}

	if(!gamePlayer->IsInGame()) {
		throw ClientNotInGameExceptionType(clientIndex);
	}

	IPlayerInfo *playerInfo = gamePlayer->GetPlayerInfo();

	if(NULL == playerInfo) {
		throw ClientDataNotAvailableExceptionType(clientIndex, "player_info"); 
	}

	return std::string(playerInfo->GetModelName());
}

std::string clients__get_client_weapon(int clientIndex) {
	if(clientIndex < 1 || clientIndex > g_Interfaces.GlobalVarsInstance->maxClients) {
		throw ClientIndexOutOfRangeExceptionType(clientIndex, g_Interfaces.GlobalVarsInstance->maxClients);
	}

	SourceMod::IGamePlayer *gamePlayer = playerhelpers->GetGamePlayer(clientIndex);

	if(!gamePlayer->IsConnected()) {
		throw ClientNotConnectedExceptionType(clientIndex);
	}

	if(!gamePlayer->IsInGame()) {
		throw ClientNotInGameExceptionType(clientIndex);
	}

	IPlayerInfo *playerInfo = gamePlayer->GetPlayerInfo();

	if(NULL == playerInfo) {
		throw ClientDataNotAvailableExceptionType(clientIndex, "player_info"); 
	}

	return std::string(playerInfo->GetWeaponName());
}

VectorType clients__get_client_maxs(int clientIndex) {
	if(clientIndex < 1 || clientIndex > g_Interfaces.GlobalVarsInstance->maxClients) {
		throw ClientIndexOutOfRangeExceptionType(clientIndex, g_Interfaces.GlobalVarsInstance->maxClients);
	}

	SourceMod::IGamePlayer *gamePlayer = playerhelpers->GetGamePlayer(clientIndex);

	if(!gamePlayer->IsConnected()) {
		throw ClientNotConnectedExceptionType(clientIndex);
	}

	if(!gamePlayer->IsInGame()) {
		throw ClientNotInGameExceptionType(clientIndex);
	}

	IPlayerInfo *playerInfo = gamePlayer->GetPlayerInfo();

	if(NULL == playerInfo) {
		throw ClientDataNotAvailableExceptionType(clientIndex, "player_info"); 
	}

	Vector maxs = playerInfo->GetPlayerMaxs();

	return VectorType(maxs.x, maxs.y, maxs.z);
}

VectorType clients__get_client_mins(int clientIndex) {
	if(clientIndex < 1 || clientIndex > g_Interfaces.GlobalVarsInstance->maxClients) {
		throw ClientIndexOutOfRangeExceptionType(clientIndex, g_Interfaces.GlobalVarsInstance->maxClients);
	}

	SourceMod::IGamePlayer *gamePlayer = playerhelpers->GetGamePlayer(clientIndex);

	if(!gamePlayer->IsConnected()) {
		throw ClientNotConnectedExceptionType(clientIndex);
	}

	if(!gamePlayer->IsInGame()) {
		throw ClientNotInGameExceptionType(clientIndex);
	}

	IPlayerInfo *playerInfo = gamePlayer->GetPlayerInfo();

	if(NULL == playerInfo) {
		throw ClientDataNotAvailableExceptionType(clientIndex, "player_info"); 
	}

	Vector mins = playerInfo->GetPlayerMins();

	return VectorType(mins.x, mins.y, mins.z);
}

VectorType clients__get_client_abs_angles(int clientIndex) {
	if(clientIndex < 1 || clientIndex > g_Interfaces.GlobalVarsInstance->maxClients) {
		throw ClientIndexOutOfRangeExceptionType(clientIndex, g_Interfaces.GlobalVarsInstance->maxClients);
	}

	SourceMod::IGamePlayer *gamePlayer = playerhelpers->GetGamePlayer(clientIndex);

	if(!gamePlayer->IsConnected()) {
		throw ClientNotConnectedExceptionType(clientIndex);
	}

	if(!gamePlayer->IsInGame()) {
		throw ClientNotInGameExceptionType(clientIndex);
	}

	IPlayerInfo *playerInfo = gamePlayer->GetPlayerInfo();

	if(NULL == playerInfo) {
		throw ClientDataNotAvailableExceptionType(clientIndex, "player_info"); 
	}

	QAngle absAngles = playerInfo->GetAbsAngles();

	return VectorType(absAngles.x, absAngles.y, absAngles.z);
}

VectorType clients__get_client_abs_origin(int clientIndex) {
	if(clientIndex < 1 || clientIndex > g_Interfaces.GlobalVarsInstance->maxClients) {
		throw ClientIndexOutOfRangeExceptionType(clientIndex, g_Interfaces.GlobalVarsInstance->maxClients);
	}

	SourceMod::IGamePlayer *gamePlayer = playerhelpers->GetGamePlayer(clientIndex);

	if(!gamePlayer->IsConnected()) {
		throw ClientNotConnectedExceptionType(clientIndex);
	}

	if(!gamePlayer->IsInGame()) {
		throw ClientNotInGameExceptionType(clientIndex);
	}

	IPlayerInfo *playerInfo = gamePlayer->GetPlayerInfo();

	if(NULL == playerInfo) {
		throw ClientDataNotAvailableExceptionType(clientIndex, "player_info"); 
	}

	Vector absOrigin = playerInfo->GetAbsOrigin();

	return VectorType(absOrigin.x, absOrigin.y, absOrigin.z);
}

int clients__get_client_armor(int clientIndex) {
	if(clientIndex < 1 || clientIndex > g_Interfaces.GlobalVarsInstance->maxClients) {
		throw ClientIndexOutOfRangeExceptionType(clientIndex, g_Interfaces.GlobalVarsInstance->maxClients);
	}

	SourceMod::IGamePlayer *gamePlayer = playerhelpers->GetGamePlayer(clientIndex);

	if(!gamePlayer->IsConnected()) {
		throw ClientNotConnectedExceptionType(clientIndex);
	}

	if(!gamePlayer->IsInGame()) {
		throw ClientNotInGameExceptionType(clientIndex);
	}

	IPlayerInfo *playerInfo = gamePlayer->GetPlayerInfo();

	if(NULL == playerInfo) {
		throw ClientDataNotAvailableExceptionType(clientIndex, "player_info"); 
	}

	return playerInfo->GetArmorValue();
}

int clients__get_client_deaths(int clientIndex) {
	if(clientIndex < 1 || clientIndex > g_Interfaces.GlobalVarsInstance->maxClients) {
		throw ClientIndexOutOfRangeExceptionType(clientIndex, g_Interfaces.GlobalVarsInstance->maxClients);
	}

	SourceMod::IGamePlayer *gamePlayer = playerhelpers->GetGamePlayer(clientIndex);

	if(!gamePlayer->IsConnected()) {
		throw ClientNotConnectedExceptionType(clientIndex);
	}

	if(!gamePlayer->IsInGame()) {
		throw ClientNotInGameExceptionType(clientIndex);
	}

	IPlayerInfo *playerInfo = gamePlayer->GetPlayerInfo();

	if(NULL == playerInfo) {
		throw ClientDataNotAvailableExceptionType(clientIndex, "player_info"); 
	}

	return playerInfo->GetDeathCount();
}

int clients__get_client_frags(int clientIndex) {
	if(clientIndex < 1 || clientIndex > g_Interfaces.GlobalVarsInstance->maxClients) {
		throw ClientIndexOutOfRangeExceptionType(clientIndex, g_Interfaces.GlobalVarsInstance->maxClients);
	}

	SourceMod::IGamePlayer *gamePlayer = playerhelpers->GetGamePlayer(clientIndex);

	if(!gamePlayer->IsConnected()) {
		throw ClientNotConnectedExceptionType(clientIndex);
	}

	if(!gamePlayer->IsInGame()) {
		throw ClientNotInGameExceptionType(clientIndex);
	}

	IPlayerInfo *playerInfo = gamePlayer->GetPlayerInfo();

	if(NULL == playerInfo) {
		throw ClientDataNotAvailableExceptionType(clientIndex, "player_info"); 
	}

	return playerInfo->GetFragCount();
}

int clients__get_client_data_rate(int clientIndex) {
	if(clientIndex < 1 || clientIndex > g_Interfaces.GlobalVarsInstance->maxClients) {
		throw ClientIndexOutOfRangeExceptionType(clientIndex, g_Interfaces.GlobalVarsInstance->maxClients);
	}

	SourceMod::IGamePlayer *gamePlayer = playerhelpers->GetGamePlayer(clientIndex);

	if(!gamePlayer->IsConnected()) {
		throw ClientNotConnectedExceptionType(clientIndex);
	}

	if(!gamePlayer->IsInGame()) {
		throw ClientNotInGameExceptionType(clientIndex);
	}

	if(gamePlayer->IsFakeClient()) {
		throw ClientIsFakeExceptionType(clientIndex);
	}

	INetChannelInfo *netChannelInfo = engine->GetPlayerNetInfo(clientIndex);

	if(NULL == netChannelInfo) {
		throw ClientDataNotAvailableExceptionType(clientIndex, "net_info");
	}

	return netChannelInfo->GetDataRate();
}

bool clients__is_client_timing_out(int clientIndex) {
	if(clientIndex < 1 || clientIndex > g_Interfaces.GlobalVarsInstance->maxClients) {
		throw ClientIndexOutOfRangeExceptionType(clientIndex, g_Interfaces.GlobalVarsInstance->maxClients);
	}

	SourceMod::IGamePlayer *gamePlayer = playerhelpers->GetGamePlayer(clientIndex);

	if(!gamePlayer->IsConnected()) {
		throw ClientNotConnectedExceptionType(clientIndex);
	}

	if(!gamePlayer->IsInGame()) {
		throw ClientNotInGameExceptionType(clientIndex);
	}

	if(gamePlayer->IsFakeClient()) {
		throw ClientIsFakeExceptionType(clientIndex);
	}

	INetChannelInfo *netChannelInfo = engine->GetPlayerNetInfo(clientIndex);

	if(NULL == netChannelInfo) {
		throw ClientDataNotAvailableExceptionType(clientIndex, "net_info");
	}

	return netChannelInfo->IsTimingOut();
}

float clients__get_client_time_connected(int clientIndex) {
	if(clientIndex < 1 || clientIndex > g_Interfaces.GlobalVarsInstance->maxClients) {
		throw ClientIndexOutOfRangeExceptionType(clientIndex, g_Interfaces.GlobalVarsInstance->maxClients);
	}

	SourceMod::IGamePlayer *gamePlayer = playerhelpers->GetGamePlayer(clientIndex);

	if(!gamePlayer->IsConnected()) {
		throw ClientNotConnectedExceptionType(clientIndex);
	}

	if(!gamePlayer->IsInGame()) {
		throw ClientNotInGameExceptionType(clientIndex);
	}

	if(gamePlayer->IsFakeClient()) {
		throw ClientIsFakeExceptionType(clientIndex);
	}

	INetChannelInfo *netChannelInfo = engine->GetPlayerNetInfo(clientIndex);

	if(NULL == netChannelInfo) {
		throw ClientDataNotAvailableExceptionType(clientIndex, "net_info");
	}

	return netChannelInfo->GetTimeConnected();
}

float clients__get_client_latency(int clientIndex, bool outgoing = false, bool incoming = false) {
	if(clientIndex < 1 || clientIndex > g_Interfaces.GlobalVarsInstance->maxClients) {
		throw ClientIndexOutOfRangeExceptionType(clientIndex, g_Interfaces.GlobalVarsInstance->maxClients);
	}

	SourceMod::IGamePlayer *gamePlayer = playerhelpers->GetGamePlayer(clientIndex);

	if(!gamePlayer->IsConnected()) {
		throw ClientNotConnectedExceptionType(clientIndex);
	}

	if(!gamePlayer->IsInGame()) {
		throw ClientNotInGameExceptionType(clientIndex);
	}

	if(gamePlayer->IsFakeClient()) {
		throw ClientIsFakeExceptionType(clientIndex);
	}

	INetChannelInfo *netChannelInfo = engine->GetPlayerNetInfo(clientIndex);

	if(NULL == netChannelInfo) {
		throw ClientDataNotAvailableExceptionType(clientIndex, "net_info");
	}

	float total = 0;

	if(outgoing) {
		total += netChannelInfo->GetLatency(FLOW_OUTGOING);
	}

	if(incoming) {
		total += netChannelInfo->GetLatency(FLOW_INCOMING);
	}

	return total;
}

float clients__get_client_avg_latency(int clientIndex, bool outgoing = false, bool incoming = false) {
	if(clientIndex < 1 || clientIndex > g_Interfaces.GlobalVarsInstance->maxClients) {
		throw ClientIndexOutOfRangeExceptionType(clientIndex, g_Interfaces.GlobalVarsInstance->maxClients);
	}

	SourceMod::IGamePlayer *gamePlayer = playerhelpers->GetGamePlayer(clientIndex);

	if(!gamePlayer->IsConnected()) {
		throw ClientNotConnectedExceptionType(clientIndex);
	}

	if(!gamePlayer->IsInGame()) {
		throw ClientNotInGameExceptionType(clientIndex);
	}

	if(gamePlayer->IsFakeClient()) {
		throw ClientIsFakeExceptionType(clientIndex);
	}

	INetChannelInfo *netChannelInfo = engine->GetPlayerNetInfo(clientIndex);

	if(NULL == netChannelInfo) {
		throw ClientDataNotAvailableExceptionType(clientIndex, "net_info");
	}

	float total = 0;

	if(outgoing) {
		total += netChannelInfo->GetAvgLatency(FLOW_OUTGOING);
	}

	if(incoming) {
		total += netChannelInfo->GetAvgLatency(FLOW_INCOMING);
	}

	return total;
}

float clients__get_client_avg_loss(int clientIndex, bool outgoing = false, bool incoming = false) {
	if(clientIndex < 1 || clientIndex > g_Interfaces.GlobalVarsInstance->maxClients) {
		throw ClientIndexOutOfRangeExceptionType(clientIndex, g_Interfaces.GlobalVarsInstance->maxClients);
	}

	SourceMod::IGamePlayer *gamePlayer = playerhelpers->GetGamePlayer(clientIndex);

	if(!gamePlayer->IsConnected()) {
		throw ClientNotConnectedExceptionType(clientIndex);
	}

	if(!gamePlayer->IsInGame()) {
		throw ClientNotInGameExceptionType(clientIndex);
	}

	if(gamePlayer->IsFakeClient()) {
		throw ClientIsFakeExceptionType(clientIndex);
	}

	INetChannelInfo *netChannelInfo = engine->GetPlayerNetInfo(clientIndex);

	if(NULL == netChannelInfo) {
		throw ClientDataNotAvailableExceptionType(clientIndex, "net_info");
	}

	float total = 0;

	if(outgoing) {
		total += netChannelInfo->GetAvgLoss(FLOW_OUTGOING);
	}

	if(incoming) {
		total += netChannelInfo->GetAvgLoss(FLOW_INCOMING);
	}

	return total;
}

float clients__get_client_avg_choke(int clientIndex, bool outgoing = false, bool incoming = false) {
	if(clientIndex < 1 || clientIndex > g_Interfaces.GlobalVarsInstance->maxClients) {
		throw ClientIndexOutOfRangeExceptionType(clientIndex, g_Interfaces.GlobalVarsInstance->maxClients);
	}

	SourceMod::IGamePlayer *gamePlayer = playerhelpers->GetGamePlayer(clientIndex);

	if(!gamePlayer->IsConnected()) {
		throw ClientNotConnectedExceptionType(clientIndex);
	}

	if(!gamePlayer->IsInGame()) {
		throw ClientNotInGameExceptionType(clientIndex);
	}

	if(gamePlayer->IsFakeClient()) {
		throw ClientIsFakeExceptionType(clientIndex);
	}

	INetChannelInfo *netChannelInfo = engine->GetPlayerNetInfo(clientIndex);

	if(NULL == netChannelInfo) {
		throw ClientDataNotAvailableExceptionType(clientIndex, "net_info");
	}

	float total = 0;

	if(outgoing) {
		total += netChannelInfo->GetAvgChoke(FLOW_OUTGOING);
	}

	if(incoming) {
		total += netChannelInfo->GetAvgChoke(FLOW_INCOMING);
	}

	return total;
}

float clients__get_client_avg_data(int clientIndex, bool outgoing = false, bool incoming = false) {
	if(clientIndex < 1 || clientIndex > g_Interfaces.GlobalVarsInstance->maxClients) {
		throw ClientIndexOutOfRangeExceptionType(clientIndex, g_Interfaces.GlobalVarsInstance->maxClients);
	}

	SourceMod::IGamePlayer *gamePlayer = playerhelpers->GetGamePlayer(clientIndex);

	if(!gamePlayer->IsConnected()) {
		throw ClientNotConnectedExceptionType(clientIndex);
	}

	if(!gamePlayer->IsInGame()) {
		throw ClientNotInGameExceptionType(clientIndex);
	}

	if(gamePlayer->IsFakeClient()) {
		throw ClientIsFakeExceptionType(clientIndex);
	}

	INetChannelInfo *netChannelInfo = engine->GetPlayerNetInfo(clientIndex);

	if(NULL == netChannelInfo) {
		throw ClientDataNotAvailableExceptionType(clientIndex, "net_info");
	}

	float total = 0;

	if(outgoing) {
		total += netChannelInfo->GetAvgData(FLOW_OUTGOING);
	}

	if(incoming) {
		total += netChannelInfo->GetAvgData(FLOW_INCOMING);
	}

	return total;
}

float clients__get_client_avg_packets(int clientIndex, bool outgoing = false, bool incoming = false) {
	if(clientIndex < 1 || clientIndex > g_Interfaces.GlobalVarsInstance->maxClients) {
		throw ClientIndexOutOfRangeExceptionType(clientIndex, g_Interfaces.GlobalVarsInstance->maxClients);
	}

	SourceMod::IGamePlayer *gamePlayer = playerhelpers->GetGamePlayer(clientIndex);

	if(!gamePlayer->IsConnected()) {
		throw ClientNotConnectedExceptionType(clientIndex);
	}

	if(!gamePlayer->IsInGame()) {
		throw ClientNotInGameExceptionType(clientIndex);
	}

	if(gamePlayer->IsFakeClient()) {
		throw ClientIsFakeExceptionType(clientIndex);
	}

	INetChannelInfo *netChannelInfo = engine->GetPlayerNetInfo(clientIndex);

	if(NULL == netChannelInfo) {
		throw ClientDataNotAvailableExceptionType(clientIndex, "net_info");
	}

	float total = 0;

	if(outgoing) {
		total += netChannelInfo->GetAvgPackets(FLOW_OUTGOING);
	}

	if(incoming) {
		total += netChannelInfo->GetAvgPackets(FLOW_INCOMING);
	}

	return total;
}

int clients__get_client_index_by_user_id(int userId) {
	int clientIndex = playerhelpers->GetClientOfUserId(userId);

	if(0 == clientIndex) {
		throw InvalidUserIdExceptionType(userId);
	}

	return clientIndex;
}

void clients__kick_client(int clientIndex, std::string disconnectMessage = std::string()) {
	SourceMod::IGamePlayer *gamePlayer = playerhelpers->GetGamePlayer(clientIndex);

	if(!gamePlayer->IsConnected()) {
		throw ClientNotConnectedExceptionType(clientIndex);
	}

	if(clients__is_client_in_kick_queue(clientIndex)) {
		return;
	}

	clients__KickQueue[clientIndex] = disconnectMessage;
}

void clients__kick_client_ex(int clientIndex, std::string disconnectMessage = std::string()) {
	SourceMod::IGamePlayer *gamePlayer = playerhelpers->GetGamePlayer(clientIndex);

	if(!gamePlayer->IsConnected()) {
		throw ClientNotConnectedExceptionType(clientIndex);
	}

	gamePlayer->Kick(disconnectMessage.c_str());
}

void clients__change_client_team(int clientIndex, int teamIndex) {
	SourceMod::IGamePlayer *gamePlayer = playerhelpers->GetGamePlayer(clientIndex);

	if(!gamePlayer->IsConnected()) {
		throw ClientNotConnectedExceptionType(clientIndex);
	}

	if(!gamePlayer->IsInGame()) {
		throw ClientNotInGameExceptionType(clientIndex);
	}

	IPlayerInfo *playerInfo = gamePlayer->GetPlayerInfo();

	if(NULL == playerInfo) {
		throw ClientDataNotAvailableExceptionType(clientIndex, "player_info"); 
	}

	playerInfo->ChangeTeam(teamIndex);
}

int clients__get_client_serial(int clientIndex) {
	SourceMod::IGamePlayer *gamePlayer = playerhelpers->GetGamePlayer(clientIndex);

	if(!gamePlayer->IsConnected()) {
		throw ClientNotConnectedExceptionType(clientIndex);
	}

	return gamePlayer->GetSerial();
}

int clients__get_client_index_from_serial(int serial) {
	int clientIndex = playerhelpers->GetClientFromSerial(serial);

	if(0 == clientIndex) {
		throw InvalidClientSerialExceptionType(serial);
	}

	return clientIndex;
}

void clients__GameFrame(bool simulating) {
	for(std::map<int, std::string>::iterator it = clients__KickQueue.begin(); it != clients__KickQueue.end(); it++) {
		std::pair<int, std::string> kickQueuePair = *it;

		SourceMod::IGamePlayer *gamePlayer = playerhelpers->GetGamePlayer(kickQueuePair.first);

		if(!gamePlayer->IsConnected()) {
			continue;
		}

		gamePlayer->Kick(kickQueuePair.second.c_str());
	}

	clients__KickQueue.clear();
}

DEFINE_CUSTOM_EXCEPTION_INIT(ClientIndexOutOfRangeExceptionType, clients)
DEFINE_CUSTOM_EXCEPTION_INIT(ClientDataNotAvailableExceptionType, clients)
DEFINE_CUSTOM_EXCEPTION_INIT(ClientNotConnectedExceptionType, clients)
DEFINE_CUSTOM_EXCEPTION_INIT(ClientNotInGameExceptionType, clients)
DEFINE_CUSTOM_EXCEPTION_INIT(ClientNotFakeExceptionType, clients)
DEFINE_CUSTOM_EXCEPTION_INIT(ClientNotAuthorizedExceptionType, clients)
DEFINE_CUSTOM_EXCEPTION_INIT(ClientIsFakeExceptionType, clients)
DEFINE_CUSTOM_EXCEPTION_INIT(InvalidUserIdExceptionType, clients)
DEFINE_CUSTOM_EXCEPTION_INIT(InvalidClientSerialExceptionType, clients)

BOOST_PYTHON_MODULE(clients) {
	py::def("get_max_clients", &clients__get_max_clients);
	py::def("get_client_count", &clients__get_client_count, (py::arg("in_game_only") = true));
	py::def("get_client_name", &clients__get_client_name, (py::arg("client_index")));
	py::def("get_client_ip", &clients__get_client_ip, (py::arg("client_index"), py::arg("remove_port") = true));
	py::def("get_client_auth_string", &clients__get_client_auth_string, (py::arg("client_index")));
	py::def("get_client_user_id", &clients__get_client_user_id, (py::arg("client_index")));
	py::def("is_client_connected", &clients__is_client_connected, (py::arg("client_index")));
	py::def("is_client_in_game", &clients__is_client_in_game, (py::arg("client_index")));
	py::def("is_client_in_kick_queue", &clients__is_client_in_kick_queue, (py::arg("client_index")));
	py::def("is_client_authorized", &clients__is_client_authorized, (py::arg("client_index")));
	py::def("is_client_fake", &clients__is_client_fake, (py::arg("client_index")));
	py::def("is_client_source_tv", &clients__is_client_source_tv, (py::arg("client_index")));
	py::def("is_client_replay", &clients__is_client_replay, (py::arg("client_index")));
	py::def("is_client_observer", &clients__is_client_observer, (py::arg("client_index")));
	py::def("is_client_alive", &clients__is_client_alive, (py::arg("client_index")));
	py::def("get_client_info", &clients__get_client_info, (py::arg("client_index"), py::arg("key")));
	py::def("get_client_team", &clients__get_client_team, (py::arg("client_index")));
	py::def("create_fake_client", &clients__create_fake_client, (py::arg("name")));
	py::def("set_fake_client_cvar", &clients__set_fake_client_cvar, (py::arg("client_index"), py::arg("cvar_name"), py::arg("new_value")));
	py::def("get_client_health", &clients__get_client_health, (py::arg("client_index")));
	py::def("get_client_model", &clients__get_client_model, (py::arg("client_index")));
	py::def("get_client_weapon", &clients__get_client_weapon, (py::arg("client_index")));
	py::def("get_client_maxs", &clients__get_client_maxs, (py::arg("client_index")));
	py::def("get_client_mins", &clients__get_client_mins, (py::arg("client_index")));
	py::def("get_client_abs_angles", &clients__get_client_abs_angles, (py::arg("client_index")));
	py::def("get_client_abs_origin", &clients__get_client_abs_origin, (py::arg("client_index")));
	py::def("get_client_armor", &clients__get_client_armor, (py::arg("client_index")));
	py::def("get_client_deaths", &clients__get_client_deaths, (py::arg("client_index")));
	py::def("get_client_frags", &clients__get_client_frags, (py::arg("client_index")));
	py::def("get_client_data_rate", &clients__get_client_data_rate, (py::arg("client_index")));
	py::def("is_client_timing_out", &clients__is_client_timing_out, (py::arg("client_index")));
	py::def("get_client_time_connected", &clients__get_client_time_connected, (py::arg("client_index")));
	py::def("get_client_latency", &clients__get_client_latency, (py::arg("client_index"), py::arg("outgoing") = false, py::arg("incoming") = false));
	py::def("get_client_avg_latency", &clients__get_client_avg_latency, (py::arg("client_index"), py::arg("outgoing") = false, py::arg("incoming") = false));
	py::def("get_client_avg_loss", &clients__get_client_avg_loss, (py::arg("client_index"), py::arg("outgoing") = false, py::arg("incoming") = false));
	py::def("get_client_avg_choke", &clients__get_client_avg_choke, (py::arg("client_index"), py::arg("outgoing") = false, py::arg("incoming") = false));
	py::def("get_client_avg_data", &clients__get_client_avg_data, (py::arg("client_index"), py::arg("outgoing") = false, py::arg("incoming") = false));
	py::def("get_client_avg_packets", &clients__get_client_avg_packets, (py::arg("client_index"), py::arg("outgoing") = false, py::arg("incoming") = false));
	py::def("get_client_index_by_user_id", &clients__get_client_index_by_user_id, (py::arg("user_id")));
	py::def("kick_client", &clients__kick_client, (py::arg("client_index"), py::arg("disconnect_reason") = std::string()));
	py::def("kick_client_ex", &clients__kick_client_ex, (py::arg("client_index"), py::arg("disconnect_reason") = std::string()));
	py::def("change_client_team", &clients__change_client_team, (py::arg("client_index"), py::arg("team_index")));
	py::def("get_client_serial", &clients__get_client_serial, (py::arg("client_index")));
	py::def("get_client_index_from_serial", &clients__get_client_index_from_serial, (py::arg("serial")));

	DEFINE_CUSTOM_EXCEPTION(ClientIndexOutOfRangeExceptionType, clients,
		PyExc_Exception, "clients.ClientIndexOutOfRangeException",
		"ClientIndexOutOfRangeException")

	DEFINE_CUSTOM_EXCEPTION(ClientDataNotAvailableExceptionType, clients,
		PyExc_Exception, "clients.ClientDataNotAvailableException",
		"ClientDataNotAvailableException")

	DEFINE_CUSTOM_EXCEPTION(ClientNotConnectedExceptionType, clients,
		PyExc_Exception, "clients.ClientNotConnectedException",
		"ClientNotConnectedException")

	DEFINE_CUSTOM_EXCEPTION(ClientNotFakeExceptionType, clients,
		PyExc_Exception, "clients.ClientNotFakeException",
		"ClientNotFakeException")

	DEFINE_CUSTOM_EXCEPTION(ClientIsFakeExceptionType, clients,
		PyExc_Exception, "clients.ClientIsFakeException",
		"ClientIsFakeException")

	DEFINE_CUSTOM_EXCEPTION(ClientNotInGameExceptionType, clients,
		PyExc_Exception, "clients.ClientNotInGameException",
		"ClientNotInGameException")

	DEFINE_CUSTOM_EXCEPTION(InvalidUserIdExceptionType, clients,
		PyExc_Exception, "clients.InvalidUserIdException",
		"InvalidUserIdException")

	DEFINE_CUSTOM_EXCEPTION(InvalidClientSerialExceptionType, clients,
		PyExc_Exception, "clients.InvalidClientSerialException",
		"InvalidClientSerialException")

	DEFINE_CUSTOM_EXCEPTION(ClientNotAuthorizedExceptionType, clients,
		PyExc_Exception, "clients.ClientNotAuthorizedException",
		"ClientNotAuthorizedException")
}

void destroyclients() {
}