#ifndef __INCLUDE_CONSOLECOMMANDINFO_H__
#define __INCLUDE_CONSOLECOMMANDINFO_H__

#include "Macros.h"
#include "HL2SDK.h"

class ConsoleCommandInfo {
public:
	ConsoleCommandInfo(std::string name, ConCommand *conCommandInstance, bool isFromViper);

	std::string Name;
	ConCommand *ConCommandInstance;
	bool IsFromViper;
};

#endif