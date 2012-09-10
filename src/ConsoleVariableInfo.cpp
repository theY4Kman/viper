#include "ConsoleVariableInfo.h"
#include "sdk/smsdk_ext.h"
#include "CompatWrappers.h"

ConsoleVariableInfo::ConsoleVariableInfo(std::string name, ConVar *conVarInstance, bool isFromViper) {
	Name = name;
	ConVarInstance = conVarInstance;
	IsFromViper = isFromViper;
}