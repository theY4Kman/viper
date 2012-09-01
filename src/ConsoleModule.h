#ifndef __INCLUDE_CONSOLEMODULE_H__
#define __INCLUDE_CONSOLEMODULE_H__

#include "Macros.h"
#include "ConsoleCommandHookInfo.h"
#include "ConsoleCommandInfo.h"
#include "ConsoleVariableInfo.h"
#include "ConsoleVariableChangedHookInfo.h"
#include "ConsoleClientVariableQueryInfo.h"

extern "C" __declspec(dllexport) void initconsole();
extern void destroyconsole();

DEFINE_CUSTOM_EXCEPTION_DECL(ConsoleVariableDoesNotExistExceptionType, console)
DEFINE_CUSTOM_EXCEPTION_DECL(ConsoleCommandDoesNotExistExceptionType, console)

extern std::vector<std::pair<int, std::string>> console__FakeClientCommandQueue;
extern std::vector<ConsoleCommandInfo> console__Commands;
extern std::vector<ConsoleCommandHookInfo> console__ServerCommandHooks;
extern std::vector<ConsoleCommandHookInfo> console__ConsoleCommandHooks;
extern std::vector<ConsoleVariableInfo> console__Variables;
extern std::vector<ConsoleVariableChangedHookInfo> console__VariableChangedHooks;
extern std::vector<ConsoleClientVariableQueryInfo> console__PendingClientVariableQueries;

extern void console__GameFrame(bool simulating);
extern bool console__VSPQueryHooked;
extern bool console__DLLQueryHooked;

#endif