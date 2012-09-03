#ifndef __INCLUDE_CONSOLEMODULE_H__
#define __INCLUDE_CONSOLEMODULE_H__

#include "Macros.h"
#include "ConsoleCommandHookInfo.h"
#include "ConsoleCommandInfo.h"
#include "ConsoleVariableInfo.h"
#include "ConsoleVariableChangedHookInfo.h"
#include "ConsoleClientVariableQueryInfo.h"
#include "BoostPythonSM.h"

extern "C" __declspec(dllexport) void initConsole();
extern void destroyConsole();
extern void unloadThreadStateConsole(PyThreadState *threadState);

DEFINE_CUSTOM_EXCEPTION_DECL(ConsoleVariableDoesNotExistExceptionType, Console)
DEFINE_CUSTOM_EXCEPTION_DECL(ConsoleCommandDoesNotExistExceptionType, Console)

extern std::vector<std::pair<int, std::string>> console__FakeClientCommandQueue;
extern std::vector<ConsoleCommandInfo> console__Commands;
extern std::list<ConsoleCommandHookInfo> console__ServerCommandHooks;
extern std::list<ConsoleCommandHookInfo> console__ConsoleCommandHooks;
extern std::vector<ConsoleVariableInfo> console__Variables;
extern std::list<ConsoleVariableChangedHookInfo> console__VariableChangedHooks;
extern std::list<ConsoleClientVariableQueryInfo> console__PendingClientVariableQueries;

extern void console__GameFrame(bool simulating);
extern bool console__VSPQueryHooked;
extern bool console__DLLQueryHooked;

#endif