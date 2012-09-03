#include "BoostPythonSM.h"
#include "ConsoleModule.h"
#include "sdk/smsdk_ext.h"
#include "Macros.h"
#include "ViperExtension.h"
#include "CompatWrappers.h"
#include "server_class.h"
#include "dt_common.h"
#include "Util.h"
#include "ClientNotConnectedExceptionType.h"
#include "ClientIndexOutOfRangeExceptionType.h"
#include "ConsoleCommandBaseAccessor.h"
#include "ConsoleVariableType.h"
#include "ConsoleVariableDoesNotExistExceptionType.h"
#include "ClientIsFakeExceptionType.h"
#include "ClientDataNotAvailableExceptionType.h"
#include "ConsoleCommandDoesNotExistExceptionType.h"

namespace py = boost::python;

std::vector<std::pair<int, std::string>> console__FakeClientCommandQueue;
std::list<ConsoleCommandHookInfo> console__ServerCommandHooks;
std::list<ConsoleCommandHookInfo> console__ConsoleCommandHooks;
std::vector<ConsoleCommandInfo> console__Commands;
std::vector<ConsoleVariableInfo> console__Variables;
std::list<ConsoleVariableChangedHookInfo> console__VariableChangedHooks;
std::list<ConsoleClientVariableQueryInfo> console__PendingClientVariableQueries;
bool console__VSPQueryHooked = false;
bool console__DLLQueryHooked = false;

#if SOURCE_ENGINE >= SE_ORANGEBOX
SH_DECL_HOOK1_void(ConCommand, Dispatch, SH_NOATTRIB, false, const CCommand &);
#else
SH_DECL_HOOK0_void(ConCommand, Dispatch, SH_NOATTRIB, false);
#endif

#if SOURCE_ENGINE >= SE_ORANGEBOX
SH_DECL_HOOK3_void(ICvar, CallGlobalChangeCallbacks, SH_NOATTRIB, false, ConVar *, const char *, float);
#else
SH_DECL_HOOK2_void(ICvar, CallGlobalChangeCallbacks, SH_NOATTRIB, false, ConVar *, const char *);
#endif

#if SOURCE_ENGINE != SE_DARKMESSIAH
SH_DECL_HOOK5_void(IServerGameDLL, OnQueryCvarValueFinished, SH_NOATTRIB, 0, QueryCvarCookie_t, edict_t *, EQueryCvarValueStatus, const char *, const char *);
SH_DECL_HOOK5_void(IServerPluginCallbacks, OnQueryCvarValueFinished, SH_NOATTRIB, 0, QueryCvarCookie_t, edict_t *, EQueryCvarValueStatus, const char *, const char *);
#endif

#if SOURCE_ENGINE >= SE_ORANGEBOX
SH_DECL_HOOK2_void(IServerGameClients, ClientCommand, SH_NOATTRIB, 0, edict_t *, const CCommand &);
#else
SH_DECL_HOOK1_void(IServerGameClients, ClientCommand, SH_NOATTRIB, 0, edict_t *);
#endif

void console__server_command(std::string command) {
	translator->SetGlobalTarget(SOURCEMOD_SERVER_LANGUAGE);

	engine->ServerCommand(command.c_str());
}

void console__insert_server_command(std::string serverCommand) {
	translator->SetGlobalTarget(SOURCEMOD_SERVER_LANGUAGE);

	InsertServerCommand(serverCommand.c_str());
}

void console__server_execute() {
	engine->ServerExecute();
}

void console__client_command(int clientIndex, std::string command) {
	if(clientIndex < 1 || clientIndex > g_Interfaces.GlobalVarsInstance->maxClients) {
		throw ClientIndexOutOfRangeExceptionType(clientIndex, g_Interfaces.GlobalVarsInstance->maxClients);
	}
	
	SourceMod::IGamePlayer *gamePlayer = playerhelpers->GetGamePlayer(clientIndex);

	if(!gamePlayer->IsConnected()) {
		throw ClientNotConnectedExceptionType(clientIndex);
	}

	translator->SetGlobalTarget(clientIndex);

	engine->ClientCommand(gamePlayer->GetEdict(), "%s", command.c_str());
}

void console__fake_client_command(int clientIndex, std::string command) {
	if(clientIndex < 1 || clientIndex > g_Interfaces.GlobalVarsInstance->maxClients) {
		throw ClientIndexOutOfRangeExceptionType(clientIndex, g_Interfaces.GlobalVarsInstance->maxClients);
	}
	
	SourceMod::IGamePlayer *gamePlayer = playerhelpers->GetGamePlayer(clientIndex);

	if(!gamePlayer->IsConnected()) {
		throw ClientNotConnectedExceptionType(clientIndex);
	}

	g_Interfaces.ServerPluginsHelperInstance->ClientCommand(gamePlayer->GetEdict(), command.c_str());
}

void console__fake_client_command_ex(int clientIndex, std::string command) {
	if(clientIndex < 1 || clientIndex > g_Interfaces.GlobalVarsInstance->maxClients) {
		throw ClientIndexOutOfRangeExceptionType(clientIndex, g_Interfaces.GlobalVarsInstance->maxClients);
	}
	
	SourceMod::IGamePlayer *gamePlayer = playerhelpers->GetGamePlayer(clientIndex);

	if(!gamePlayer->IsConnected()) {
		throw ClientNotConnectedExceptionType(clientIndex);
	}

	console__FakeClientCommandQueue.push_back(std::pair<int, std::string>(clientIndex, command));
}

void console__print_to_server(std::string text, bool appendNewline = true) {
	if(appendNewline) {
		text.append("\n");
	}

	META_CONPRINT(text.c_str());
}

void console__print_to_console(int clientIndex, std::string text, bool appendNewline = true) {
	if(clientIndex == 0) {
		console__print_to_server(text, appendNewline);
		return;
	}
	
	if(appendNewline) {
		text.append("\n");
	}

	if(clientIndex < 1 || clientIndex > g_Interfaces.GlobalVarsInstance->maxClients) {
		throw ClientIndexOutOfRangeExceptionType(clientIndex, g_Interfaces.GlobalVarsInstance->maxClients);
	}
	
	SourceMod::IGamePlayer *gamePlayer = playerhelpers->GetGamePlayer(clientIndex);

	if(!gamePlayer->IsConnected()) {
		throw ClientNotConnectedExceptionType(clientIndex);
	}

	if(gamePlayer->IsFakeClient()) {
		return;
	}

	engine->ClientPrintf(gamePlayer->GetEdict(), text.c_str());
}

// Returns true if it should supercede
bool console__CommandHandler(int clientIndex, const CCommand &command) {
	// first extract all the command information
	std::string baseCommand(command.Arg(0));

	std::string commandString(command.GetCommandString());

	py::list argumentList;
	
	int argumentCount = command.ArgC();

	for(int argumentIndex = 0; argumentIndex < argumentCount; argumentIndex++) {
		argumentList.append<std::string>(std::string(command.Arg(argumentIndex)));
	}

	bool supercede = false;

	// then go through the server hooks if index is 0
	if(0 == clientIndex) {
		for(std::list<ConsoleCommandHookInfo>::iterator it = console__ServerCommandHooks.begin();
			it != console__ServerCommandHooks.end(); it++) {
			ConsoleCommandHookInfo commandHookInfo = *it;

			if(commandHookInfo.Name != baseCommand) {
				continue;
			}

			PyThreadState *oldThreadState = PyThreadState_Get();

			PyThreadState_Swap(commandHookInfo.ThreadState);

			SourceMod::ResultType resultType = SourceMod::Pl_Continue;

			try {
				resultType = py::extract<SourceMod::ResultType>(commandHookInfo.CallbackFunction(argumentList, commandString));
			}
			catch(const py::error_already_set &) {
				PyErr_Print();
			}

			PyThreadState_Swap(oldThreadState);

			if(resultType == SourceMod::Pl_Handled) {
				supercede = true;
				continue;
			}

			if(resultType == SourceMod::Pl_Stop) {
				return true;
			}
		}
	}

	for(std::list<ConsoleCommandHookInfo>::iterator it = console__ConsoleCommandHooks.begin();
		it != console__ConsoleCommandHooks.end(); it++) {
		ConsoleCommandHookInfo commandHookInfo = *it;

		if(commandHookInfo.Name != baseCommand) {
			continue;
		}

		PyThreadState *oldThreadState = PyThreadState_Get();

		PyThreadState_Swap(commandHookInfo.ThreadState);

		SourceMod::ResultType resultType = SourceMod::Pl_Continue;

		try {
			resultType = py::extract<SourceMod::ResultType>(commandHookInfo.CallbackFunction(clientIndex, argumentList, commandString));
		}
		catch(const py::error_already_set &) {
			PyErr_Print();
		}

		PyThreadState_Swap(oldThreadState);

		if(resultType == SourceMod::Pl_Handled) {
			supercede = true;
			continue;
		}

		if(resultType == SourceMod::Pl_Stop) {
			return true;
		}
	}

	return supercede;
}

#if SOURCE_ENGINE >= SE_ORANGEBOX
void console__OnConCommandDispatch(const CCommand &command) {
#else
void console__OnConCommandDispatch() {
	CCommand command;
#endif

	console__CommandHandler(0, command);
}

#if SOURCE_ENGINE >= SE_ORANGEBOX
void console__OnClientCommand(edict_t *edict, const CCommand &command) {
#else
void console__OnClientCommand(edict_t *edict) {
	CCommand args;
#endif
	
	SourceMod::IGamePlayer *gamePlayer = playerhelpers->GetGamePlayer(edict);

	if(!gamePlayer->IsConnected()) {
		return;
	}

	int clientIndex = IndexOfEdict(edict);

	bool supercede = console__CommandHandler(clientIndex, command);

	if(supercede) {
		RETURN_META(MRES_SUPERCEDE);
	}
}

ConsoleVariableType console__get_console_variable_by_name(std::string name) {
	// did we create this variable already?
	int variableCount = console__Variables.size();
	
	for(int variableIndex = 0; variableIndex < variableCount; variableIndex++) {
		ConsoleVariableInfo variableInfo = console__Variables[variableIndex];

		if(variableInfo.Name != name) {
			continue;
		}

		return ConsoleVariableType(variableInfo.Name, variableInfo.ConVarInstance, variableInfo.IsFromViper);
	}

	// if not, if may still already exist
	ConVar *conVar = icvar->FindVar(name.c_str());

	if(conVar != NULL) {
		return ConsoleVariableType(name, conVar, false);
	}

	// otherwise, exception
	throw ConsoleVariableDoesNotExistExceptionType(name);
}

#if SOURCE_ENGINE >= SE_ORANGEBOX
void console__OnConVarChanged(ConVar *conVar, const char *oldValue, float flOldValue) {
#else
void console__OnConVarChanged(ConVar *conVar, const char *oldValue) {
#endif
	if(strcmp(conVar->GetString(), oldValue) == 0) {
		return;
	}
	
	std::string name(conVar->GetName());

	ConsoleVariableType consoleVariable = console__get_console_variable_by_name(name);

	for(std::list<ConsoleVariableChangedHookInfo>::iterator it = console__VariableChangedHooks.begin();
		it != console__VariableChangedHooks.end(); it++) {
		ConsoleVariableChangedHookInfo variableChangedHookInfo = *it;

		if(variableChangedHookInfo.Name != name) {
			continue;
		}

		PyThreadState *currentThreadState = PyThreadState_Get();

		PyThreadState_Swap(variableChangedHookInfo.ThreadState);

		try {
			variableChangedHookInfo.CallbackFunction(consoleVariable, oldValue);
		}
		catch(const py::error_already_set &) {
			PyErr_Print();
		}

		PyThreadState_Swap(currentThreadState);
	}
}

#if SOURCE_ENGINE != SE_DARKMESSIAH
void console__OnQueryCvarValueFinished(QueryCvarCookie_t cookie, edict_t *pPlayer, EQueryCvarValueStatus result,
	const char *cvarName, const char *cvarValue) {
	// get the index of this player
	int clientIndex = IndexOfEdict(pPlayer);

	// loop through the remaining queries and find the one we are looking for
	for(std::list<ConsoleClientVariableQueryInfo>::iterator it = console__PendingClientVariableQueries.begin();
		it != console__PendingClientVariableQueries.end(); it++) {
		ConsoleClientVariableQueryInfo queryInfo = *it;

		if(queryInfo.Cookie != cookie) {
			continue;
		}

		PyThreadState *currentThreadState = PyThreadState_Get();

		PyThreadState_Swap(queryInfo.ThreadState);

		try {
			if(result == eQueryCvarValueStatus_ValueIntact) {
				queryInfo.CallbackFunction(clientIndex, std::string(cvarName), std::string(cvarValue));
			}
			else {
				queryInfo.CallbackFunction(clientIndex, std::string(cvarName), std::string());
			}
		}
		catch(const py::error_already_set &) {
			PyErr_Print();
		}

		PyThreadState_Swap(currentThreadState);

		console__PendingClientVariableQueries.erase(it);
		return;
	}
}
#endif

char *sm_strdup(const char *str)
{
	char *ptr = new char[strlen(str)+1];
	strcpy(ptr, str);
	return ptr;
}

ConsoleCommandInfo console__AddOrFindCommand(std::string command, std::string description = std::string(), int flags = 0) {
	// maybe we have it in our system
	for(std::vector<ConsoleCommandInfo>::iterator it = console__Commands.begin(); it != console__Commands.end(); it++) {
		ConsoleCommandInfo commandInfo = *it;

		if(commandInfo.Name != command) {
			continue;
		}

		return commandInfo;
	}

	// it still might exist, we just probably didn't register it through viper
	ConCommand *conCommand = FindCommand(command.c_str());

	if(conCommand != NULL) {
		// make sure to hook the command's dispatch
		SH_ADD_HOOK(ConCommand, Dispatch, conCommand, SH_STATIC(&console__OnConCommandDispatch), false);

		ConsoleCommandInfo commandInfo(command, conCommand, false);

		return commandInfo;
	}

	// okay, it doesn't exist at all yet.
	conCommand = new ConCommand(sm_strdup(command.c_str()), &console__OnConCommandDispatch, sm_strdup(description.c_str()), flags);
	
	ConsoleCommandInfo commandInfo(command, conCommand, true);

	console__Commands.push_back(commandInfo);

	return commandInfo;
}

ConsoleVariableInfo console__AddOrFindVariable(std::string name, std::string defaultValue, std::string description = std::string(),
	int flags = 0, bool hasMinimum = false, float minimum = 0.0f, bool hasMaximum = false, float maximum = 0.0f) {
	// did we create this variable already?
	int variableCount = console__Variables.size();
	
	for(int variableIndex = 0; variableIndex < variableCount; variableIndex++) {
		ConsoleVariableInfo variableInfo = console__Variables[variableIndex];

		if(variableInfo.Name != name) {
			continue;
		}

		return variableInfo;
	}

	// if not, if may still already exist
	ConVar *conVar = icvar->FindVar(name.c_str());

	if(conVar != NULL) {
		ConsoleVariableInfo variableInfo(name, conVar, false);

		return variableInfo;
	}

	// otherwise, let's create it.
	conVar = new ConVar(sm_strdup(name.c_str()), sm_strdup(defaultValue.c_str()), flags,
		sm_strdup(description.c_str()), hasMinimum, minimum, hasMaximum, maximum);

	ConsoleVariableInfo variableInfo(name, conVar, true);

	console__Variables.push_back(variableInfo);

	return variableInfo;
}

void console__register_server_command(std::string command, py::object callbackFunction, std::string description = std::string(), int flags = 0) {
	ConsoleCommandInfo commandInfo = console__AddOrFindCommand(command, description, flags);

	PyThreadState *threadState = PyThreadState_Get();

	ConsoleCommandHookInfo hookInfo(command, threadState, callbackFunction);

	console__ServerCommandHooks.push_back(hookInfo);
}

void console__register_console_command(std::string command, py::object callbackFunction, std::string description = std::string(), int flags = 0) {
	ConsoleCommandInfo commandInfo = console__AddOrFindCommand(command, description, flags);

	PyThreadState *threadState = PyThreadState_Get();

	ConsoleCommandHookInfo hookInfo(command, threadState, callbackFunction);

	console__ConsoleCommandHooks.push_back(hookInfo);
}

ConsoleVariableType console__create_console_variable(std::string name, std::string defaultValue, std::string description = std::string(),
	int flags = 0, bool hasMinimum = false, float minimum = 0.0f, bool hasMaximum = false, float maximum = 0.0f) {
	ConsoleVariableInfo variableInfo = console__AddOrFindVariable(name, defaultValue, description, flags,
		hasMinimum, minimum, hasMaximum, maximum);

	return ConsoleVariableType(variableInfo.Name, variableInfo.ConVarInstance, variableInfo.IsFromViper);
}

bool console__console_variable_exists(std::string name) {
	return icvar->FindVar(name.c_str()) ? true : false;
}

bool console__console_command_exists(std::string name) {
	ConCommand *conCommand = FindCommand(name.c_str());

	return conCommand ? true : false;
}

void console__query_client_console_variable(int clientIndex, std::string name, py::object callbackFunction) {
	if(clientIndex < 1 || clientIndex > g_Interfaces.GlobalVarsInstance->maxClients) {
		throw ClientIndexOutOfRangeExceptionType(clientIndex, g_Interfaces.GlobalVarsInstance->maxClients);
	}

	SourceMod::IGamePlayer *gamePlayer = playerhelpers->GetGamePlayer(clientIndex);

	if(!gamePlayer->IsConnected()) {
		throw ClientNotConnectedExceptionType(clientIndex);
	}

	if(gamePlayer->IsFakeClient()) {
		throw ClientIsFakeExceptionType(clientIndex);
	}

	QueryCvarCookie_t cookie = 0;

#if SOURCE_ENGINE != SE_DARKMESSIAH
	if (console__DLLQueryHooked) {
		cookie = engine->StartQueryCvarValue(gamePlayer->GetEdict(), name.c_str());	
	}
	else if (console__VSPQueryHooked) {
		cookie = g_Interfaces.ServerPluginsHelperInstance->StartQueryCvarValue(gamePlayer->GetEdict(), name.c_str());
	}
#endif

	if(cookie <= 0) {
		throw ClientDataNotAvailableExceptionType(clientIndex, "query_cvar");
	}

	PyThreadState *threadState = PyThreadState_Get();

	ConsoleClientVariableQueryInfo queryInfo(cookie, threadState, callbackFunction);

	console__PendingClientVariableQueries.push_back(queryInfo);
}

int console__get_command_flags(std::string name) {
	ConCommand *command = FindCommand(name.c_str());

	if(NULL == command) {
		throw ConsoleCommandDoesNotExistExceptionType(name);
	}

	return command->GetFlags();
}

void console__set_command_flags(std::string name, int flags) {
	ConCommand *command = FindCommand(name.c_str());

	if(NULL == command) {
		throw ConsoleCommandDoesNotExistExceptionType(name);
	}

	return command->SetFlags(flags);
}

void console__GameFrame(bool simulating) {
	for(std::vector<std::pair<int, std::string>>::iterator it = console__FakeClientCommandQueue.begin(); it != console__FakeClientCommandQueue.end(); it++) {
		std::pair<int, std::string> fakeClientCommandPair = *it;

		int clientIndex = fakeClientCommandPair.first;
		std::string command = fakeClientCommandPair.second;

		if(clientIndex < 1 || clientIndex > g_Interfaces.GlobalVarsInstance->maxClients) {
			continue;
		}
	
		SourceMod::IGamePlayer *gamePlayer = playerhelpers->GetGamePlayer(clientIndex);

		if(!gamePlayer->IsConnected()) {
			continue;
		}

		g_Interfaces.ServerPluginsHelperInstance->ClientCommand(gamePlayer->GetEdict(), command.c_str());
	}

	console__FakeClientCommandQueue.clear();
}

DEFINE_CUSTOM_EXCEPTION_INIT(ConsoleVariableDoesNotExistExceptionType, Console)
DEFINE_CUSTOM_EXCEPTION_INIT(ConsoleCommandDoesNotExistExceptionType, Console)

BOOST_PYTHON_MODULE(Console) {
	py::class_<ConsoleVariableType>("ConsoleVariable", py::no_init)
		.def("GetBool", &ConsoleVariableType::GetBool)
		.def("GetInt", &ConsoleVariableType::GetInt)
		.def("GetFloat", &ConsoleVariableType::GetFloat)
		.def("GetString", &ConsoleVariableType::GetString)
		.def("SetBool", &ConsoleVariableType::SetBool, (py::arg("value"), py::arg("replicate") = false, py::arg("notify") = false))
		.def("SetInt", &ConsoleVariableType::SetInt, (py::arg("value"), py::arg("replicate") = false, py::arg("notify") = false))
		.def("SetFloat", &ConsoleVariableType::SetFloat, (py::arg("value"), py::arg("replicate") = false, py::arg("notify") = false))
		.def("SetString", &ConsoleVariableType::SetString, (py::arg("value"), py::arg("replicate") = false, py::arg("notify") = false))
		.def("Reset", &ConsoleVariableType::Reset, (py::arg("replicate") = false, py::arg("notify") = false))
		.def("GetDefault", &ConsoleVariableType::GetDefault)
		.def("GetFlags", &ConsoleVariableType::GetFlags)
		.def("SetFlags", &ConsoleVariableType::SetFlags, (py::arg("flags")))
		.def("HasMinimum", &ConsoleVariableType::HasMinimum)
		.def("HasMaximum", &ConsoleVariableType::HasMaximum)
		.def("SetMinimum", &ConsoleVariableType::SetMinimum, (py::arg("has_minimum"), py::arg("minimum") = 0.0f))
		.def("SetMaximum", &ConsoleVariableType::SetMaximum, (py::arg("has_maximum"), py::arg("maximum") = 0.0f))
		.def("GetName", &ConsoleVariableType::GetName)
		.def("SendValueToClient", &ConsoleVariableType::SendValueToClient, (py::arg("client_index"), py::arg("value")))
		.def("HookChange", &ConsoleVariableType::HookChange, (py::arg("callback")))
		.def("UnhookChange", &ConsoleVariableType::UnhookChange, (py::arg("callback")));

	py::def("ServerCommand", &console__server_command, (py::arg("command")));
	py::def("InsertServerCommand", &console__insert_server_command, (py::arg("command")));
	py::def("ServerExecute", &console__server_execute);
	py::def("ClientCommand", &console__client_command, (py::arg("client_index"), py::arg("command")));
	py::def("FakeClientCommand", &console__fake_client_command, (py::arg("client_index"), py::arg("command")));
	py::def("FakeClientCommandEx", &console__fake_client_command_ex, (py::arg("client_index"), py::arg("command")));
	py::def("PrintToServer", &console__print_to_server, (py::arg("text"), py::arg("append_newline") = true));
	py::def("PrintToConsole", &console__print_to_console, (py::arg("client_index"), py::arg("text"), py::arg("append_newline") = true));
	py::def("RegisterServerCommand", &console__register_server_command, (py::arg("command"), py::arg("callback"), py::arg("description") = std::string(), py::arg("flags") = 0));
	py::def("RegisterConsoleCommand", &console__register_console_command, (py::arg("command"), py::arg("callback"), py::arg("description") = std::string(), py::arg("flags") = 0));
	py::def("CreateConsoleVariable", &console__create_console_variable, (py::arg("name"), py::arg("default_value"), py::arg("description") = std::string(), py::arg("flags") = 0, py::arg("has_minimum") = false, py::arg("minimum") = 0.0f, py::arg("has_maximum") = false, py::arg("maximum") = 0.0f));
	py::def("GetConsoleVariableByName", &console__get_console_variable_by_name, (py::arg("name")));
	py::def("ConsoleVariableExists", &console__console_variable_exists, (py::arg("name")));
	py::def("ConsoleCommandExists", &console__console_command_exists, (py::arg("name")));
	py::def("QueryClientConsoleVariable", &console__query_client_console_variable, (py::arg("client_index"), py::arg("name"), py::arg("callback")));
	py::def("GetCommandFlags", &console__get_command_flags, (py::arg("name")));
	py::def("SetCommandFlags", &console__set_command_flags, (py::arg("name"), py::arg("flags")));

	DEFINE_CUSTOM_EXCEPTION(ConsoleVariableDoesNotExistExceptionType, Console,
		PyExc_Exception, "Console.ConsoleVariableDoesNotExistException",
		"ConsoleVariableDoesNotExistException")

	DEFINE_CUSTOM_EXCEPTION(ConsoleCommandDoesNotExistExceptionType, Console,
		PyExc_Exception, "Console.ConsoleCommandDoesNotExistException",
		"ConsoleCommandDoesNotExistException")

	#if SOURCE_ENGINE != SE_DARKMESSIAH
	SH_ADD_HOOK(IServerPluginCallbacks, OnQueryCvarValueFinished, g_Interfaces.ServerPluginCallbacksInstance,
		SH_STATIC(&console__OnQueryCvarValueFinished), false);
	console__VSPQueryHooked = true;
	#endif
	
	#if SOURCE_ENGINE == SE_EPISODEONE
	if(g_SMAPI->GetGameDLLVersion() >= 6) {
		SH_ADD_HOOK(IServerGameDLL, OnQueryCvarValueFinished, gamedll,
			SH_STATIC(&console__OnQueryCvarValueFinished), false);
		console__DLLQueryHooked = true;
	}
	#endif

	SH_ADD_HOOK(IServerGameClients, ClientCommand, g_Interfaces.ServerGameClientsInstance,
		SH_STATIC(&console__OnClientCommand), false);

	SH_ADD_HOOK(ICvar, CallGlobalChangeCallbacks, icvar,
		SH_STATIC(&console__OnConVarChanged), false);

	CONVAR_REGISTER(&console__CommandBaseAccessor);
}

void destroyConsole() {
}

bool RemoveFirstConsoleCommandHookByThreadState(PyThreadState *threadState) {
	for(std::list<ConsoleCommandHookInfo>::iterator it = console__ConsoleCommandHooks.begin();
		it != console__ConsoleCommandHooks.end(); it++) {
		ConsoleCommandHookInfo hookInfo = *it;

		if(hookInfo.ThreadState != threadState) {
			continue;
		}

		console__ConsoleCommandHooks.erase(it);
		return true;
	}

	return false;
}

void RemoveAllConsoleCommandHooksByThreadState(PyThreadState *threadState) {
	bool keepSearching = true;

	while(keepSearching) {
		keepSearching = RemoveFirstConsoleCommandHookByThreadState(threadState);
	}
}

bool RemoveFirstServerCommandHookByThreadState(PyThreadState *threadState) {
	for(std::list<ConsoleCommandHookInfo>::iterator it = console__ServerCommandHooks.begin();
		it != console__ServerCommandHooks.end(); it++) {
		ConsoleCommandHookInfo hookInfo = *it;

		if(hookInfo.ThreadState != threadState) {
			continue;
		}

		console__ServerCommandHooks.erase(it);
		return true;
	}

	return false;
}

void RemoveAllServerCommandHooksByThreadState(PyThreadState *threadState) {
	bool keepSearching = true;

	while(keepSearching) {
		keepSearching = RemoveFirstServerCommandHookByThreadState(threadState);
	}
}

bool RemoveFirstVariableChangedHookByThreadState(PyThreadState *threadState) {
	for(std::list<ConsoleVariableChangedHookInfo>::iterator it = console__VariableChangedHooks.begin();
		it != console__VariableChangedHooks.end(); it++) {
		ConsoleVariableChangedHookInfo hookInfo = *it;

		if(hookInfo.ThreadState != threadState) {
			continue;
		}

		console__VariableChangedHooks.erase(it);
		return true;
	}

	return false;
}

void RemoveAllVariableChangedHooksByThreadState(PyThreadState *threadState) {
	bool keepSearching = true;

	while(keepSearching) {
		keepSearching = RemoveFirstVariableChangedHookByThreadState(threadState);
	}
}

bool RemoveFirstPendingClientVariableQueryByThreadState(PyThreadState *threadState) {
	for(std::list<ConsoleClientVariableQueryInfo>::iterator it = console__PendingClientVariableQueries.begin();
		it != console__PendingClientVariableQueries.end(); it++) {
		ConsoleClientVariableQueryInfo hookInfo = *it;

		if(hookInfo.ThreadState != threadState) {
			continue;
		}

		console__PendingClientVariableQueries.erase(it);
		return true;
	}

	return false;
}

void RemoveAllPendingClientVariableQueriesByThreadState(PyThreadState *threadState) {
	bool keepSearching = true;

	while(keepSearching) {
		keepSearching = RemoveFirstPendingClientVariableQueryByThreadState(threadState);
	}
}

void unloadThreadStateConsole(PyThreadState *threadState) {
	RemoveAllConsoleCommandHooksByThreadState(threadState);
	RemoveAllServerCommandHooksByThreadState(threadState);
	RemoveAllVariableChangedHooksByThreadState(threadState);
	RemoveAllPendingClientVariableQueriesByThreadState(threadState);
}