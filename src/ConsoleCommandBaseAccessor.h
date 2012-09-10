#ifndef __INCLUDE_CONSOLECOMMANDBASEACCESSOR_H__
#define __INCLUDE_CONSOLECOMMANDBASEACCESSOR_H__

#include "HL2SDK.h"

class ConsoleCommandBaseAccessor : public IConCommandBaseAccessor {
public:
	bool RegisterConCommandBase(ConCommandBase *conCommandBase);
};

extern ConsoleCommandBaseAccessor console__CommandBaseAccessor;

#endif