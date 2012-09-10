#ifndef __INCLUDE_VIPERROOTCONSOLECOMMAND_H__
#define __INCLUDE_VIPERROOTCONSOLECOMMAND_H__

#include "STL.h"
#include "HL2SDK.h"
#include "CompatWrappers.h"

class ViperRootConsoleCommand : public SourceMod::IRootConsoleCommand {
public:
	void OnRootConsoleCommand2(const char *commandName, const ICommandArgs *commandArgs);
	void ListAllCommands();
	void ListPluginCommands();
};

#endif