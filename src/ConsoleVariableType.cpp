#include "BoostPythonSM.h"
#include "ConsoleVariableType.h"
#include "sdk/smsdk_ext.h"
#include "Macros.h"
#include "ViperExtension.h"
#include "CompatWrappers.h"
#include "inetchannel.h"
#include "ClientIndexOutOfRangeExceptionType.h"
#include "ClientNotConnectedExceptionType.h"
#include "ClientIsFakeExceptionType.h"
#include "ClientDataNotAvailableExceptionType.h"
#include "ConsoleModule.h"

namespace py = boost::python;

#ifdef CreateEvent
#undef CreateEvent
#endif

#if SOURCE_ENGINE >= SE_EYE
#define NETMSG_BITS 6
#else
#define NETMSG_BITS 5
#endif

#if SOURCE_ENGINE >= SE_LEFT4DEAD
#define NET_SETCONVAR	6
#else
#define NET_SETCONVAR	5
#endif


#if SOURCE_ENGINE < SE_ORANGEBOX
static void ReplicateConVar(ConVar *pConVar) {
	int maxClients = g_Interfaces.GlobalVarsInstance->maxClients;

	char data[256];
	bf_write buffer(data, sizeof(data));

	buffer.WriteUBitLong(NET_SETCONVAR, NETMSG_BITS);
	buffer.WriteByte(1);
	buffer.WriteString(pConVar->GetName());
	buffer.WriteString(pConVar->GetString());

	for(int i = 1; i <= maxClients; i++) {
		IGamePlayer *gamePlayer = playerhelpers->GetGamePlayer(i);

		if (gamePlayer->IsConnected() && gamePlayer->IsInGame() && !gamePlayer->IsFakeClient()) {
			if (INetChannel *netchan = static_cast<INetChannel *>(engine->GetPlayerNetInfo(i)))
				netchan->SendData(buffer);
		}
	}
}

static void NotifyConVar(ConVar *pConVar) {
	IGameEvent *pEvent = g_Interfaces.GameEventManagerInstance->CreateEvent("server_cvar");
	
	pEvent->SetString("cvarname", pConVar->GetName());
	
	if (IsFlagSet(pConVar, FCVAR_PROTECTED)) {
		pEvent->SetString("cvarvalue", "***PROTECTED***");
	}
	else {
		pEvent->SetString("cvarvalue", pConVar->GetString());
	}

	g_Interfaces.GameEventManagerInstance->FireEvent(pEvent);
}
#endif

ConsoleVariableType::ConsoleVariableType(std::string name, ConVar *conVarInstance, bool isFromViper) {
	Name = name;
	ConVarInstance = conVarInstance;
	IsFromViper = isFromViper;
}

bool ConsoleVariableType::GetBool() {
	return ConVarInstance->GetBool();
}

int ConsoleVariableType::GetInt() {
	return ConVarInstance->GetInt();
}

float ConsoleVariableType::GetFloat() {
	return ConVarInstance->GetFloat();
}

std::string ConsoleVariableType::GetString() {
	return std::string(ConVarInstance->GetString());
}

void ConsoleVariableType::SetBool(bool value, bool replicate, bool notify) {
	ConVarInstance->SetValue(value ? 1 : 0);

#if SOURCE_ENGINE < SE_ORANGEBOX
	if(replicate && IsFlagSet(ConVarInstance, FCVAR_REPLICATED)) {
		ReplicateConVar(ConVarInstance);
	}

	if(notify && IsFlagSet(ConVarInstance, FCVAR_NOTIFY)) {
		NotifyConVar(ConVarInstance);
	}
#endif
}

void ConsoleVariableType::SetInt(int value, bool replicate, bool notify) {
	ConVarInstance->SetValue(value);

#if SOURCE_ENGINE < SE_ORANGEBOX
	if(replicate && IsFlagSet(ConVarInstance, FCVAR_REPLICATED)) {
		ReplicateConVar(ConVarInstance);
	}

	if(notify && IsFlagSet(ConVarInstance, FCVAR_NOTIFY)) {
		NotifyConVar(ConVarInstance);
	}
#endif
}

void ConsoleVariableType::SetFloat(float value, bool replicate, bool notify) {
	ConVarInstance->SetValue(value);

#if SOURCE_ENGINE < SE_ORANGEBOX
	if(replicate && IsFlagSet(ConVarInstance, FCVAR_REPLICATED)) {
		ReplicateConVar(ConVarInstance);
	}

	if(notify && IsFlagSet(ConVarInstance, FCVAR_NOTIFY)) {
		NotifyConVar(ConVarInstance);
	}
#endif
}

void ConsoleVariableType::SetString(std::string value, bool replicate, bool notify) {
	ConVarInstance->SetValue(value.c_str());

#if SOURCE_ENGINE < SE_ORANGEBOX
	if(replicate && IsFlagSet(ConVarInstance, FCVAR_REPLICATED)) {
		ReplicateConVar(ConVarInstance);
	}

	if(notify && IsFlagSet(ConVarInstance, FCVAR_NOTIFY)) {
		NotifyConVar(ConVarInstance);
	}
#endif
}

void ConsoleVariableType::Reset(bool replicate, bool notify) {
	ConVarInstance->Revert();

#if SOURCE_ENGINE < SE_ORANGEBOX
	if(replicate && IsFlagSet(ConVarInstance, FCVAR_REPLICATED)) {
		ReplicateConVar(ConVarInstance);
	}

	if(notify && IsFlagSet(ConVarInstance, FCVAR_NOTIFY)) {
		NotifyConVar(ConVarInstance);
	}
#endif
}

std::string ConsoleVariableType::GetDefault() {
	return std::string(ConVarInstance->GetDefault());
}

int ConsoleVariableType::GetFlags() {
	return ConVarInstance->GetFlags();
}

void ConsoleVariableType::SetFlags(int flags) {
	ConVarInstance->SetFlags(flags);
}

bool ConsoleVariableType::HasMinimum() {
	float min;

	bool hasMin = ConVarInstance->GetMin(min);

	return hasMin;
}

bool ConsoleVariableType::HasMaximum() {
	float max;

	bool hasMax = ConVarInstance->GetMax(max);

	return hasMax;
}

void ConsoleVariableType::SetMinimum(bool hasMinimum, float minimum) {
	ConVarInstance->SetMin(hasMinimum, minimum);
}

void ConsoleVariableType::SetMaximum(bool hasMaximum, float maximum) {
	ConVarInstance->SetMax(hasMaximum, maximum);
}

std::string ConsoleVariableType::GetName() {
	return std::string(ConVarInstance->GetName());
}

void ConsoleVariableType::SendValueToClient(int clientIndex, std::string value) {
	if(clientIndex < 1 || clientIndex > g_Interfaces.GlobalVarsInstance->maxClients) {
		throw ClientIndexOutOfRangeExceptionType(clientIndex, g_Interfaces.GlobalVarsInstance->maxClients);
	}

	char data[256];
	bf_write buffer(data, sizeof(data));

	buffer.WriteUBitLong(NET_SETCONVAR, NETMSG_BITS);
	buffer.WriteByte(1);
	buffer.WriteString(ConVarInstance->GetName());
	buffer.WriteString(value.c_str());

	IGamePlayer *gamePlayer = playerhelpers->GetGamePlayer(clientIndex);

	if(!gamePlayer->IsConnected()) {
		throw ClientNotConnectedExceptionType(clientIndex);
	}

	if(gamePlayer->IsFakeClient()) {
		throw ClientIsFakeExceptionType(clientIndex);
	}

	INetChannel *netchan = static_cast<INetChannel *>(engine->GetPlayerNetInfo(clientIndex));
	
	if (netchan == NULL) {
		throw ClientDataNotAvailableExceptionType(clientIndex, "net_chan");
	}

	netchan->SendData(buffer);
}

void ConsoleVariableType::HookChange(boost::python::object callbackFunction) {
	PyThreadState *threadState = PyThreadState_Get();

	ConsoleVariableChangedHookInfo hookInfo(Name, threadState, callbackFunction);

	console__VariableChangedHooks.push_back(hookInfo);
}

void ConsoleVariableType::UnhookChange(boost::python::object callbackFunction) {
	for(std::list<ConsoleVariableChangedHookInfo>::iterator it = console__VariableChangedHooks.begin();
		it != console__VariableChangedHooks.end(); it++) {
		ConsoleVariableChangedHookInfo hookInfo = *it;
		
		if(hookInfo.Name != Name || hookInfo.CallbackFunction != callbackFunction) {
			continue;
		}

		console__VariableChangedHooks.erase(it);
		return;
	}
}