#ifndef __INCLUDE_CONSOLEVARIABLEINFO_H__
#define __INCLUDE_CONSOLEVARIABLEINFO_H__

#include "STL.h"
#include "HL2SDK.h"

class ConsoleVariableInfo {
public:
	ConsoleVariableInfo(std::string name, ConVar *conVarInstance, bool isFromViper);

	std::string Name;
	ConVar *ConVarInstance;
	bool IsFromViper;
};

#endif