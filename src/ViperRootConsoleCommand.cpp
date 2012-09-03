#include "BoostPythonSM.h"
#include "ViperRootConsoleCommand.h"
#include "Macros.h"
#include "CompatWrappers.h"
#include "ViperExtension.h"

namespace py = boost::python;

void ViperRootConsoleCommand::OnRootConsoleCommand2(const char *commandName, const ICommandArgs *commandArgs) {
	int argCount = commandArgs->ArgC();

	if(argCount == 2) {
		ListAllCommands();
		return;
	}

	std::string viperCommand(commandArgs->Arg(2));

	if(viperCommand == "plugins") {
		if(argCount == 3) {
			ListPluginCommands();
			return;
		}

		std::string subCommand(commandArgs->Arg(3));

		if(subCommand == "load") {
			if(argCount != 5) {
				g_SMAPI->ConPrint("Usage: sm py plugins load <plugin path>\n");
				return;
			}

			std::string loadStr(commandArgs->Arg(4));

			char fullPath[PLATFORM_MAX_PATH];

			g_pSM->BuildPath(SourceMod::Path_SM, fullPath, sizeof(fullPath), "plugins/%s/__init__.py", loadStr.c_str());

			std::string fullPathStr(fullPath);

			try {
				g_ViperExtension.GetPluginManager()->LoadPlugin(fullPathStr);

				g_SMAPI->ConPrintf("Plugin \"%s\" loaded.\n", loadStr.c_str());
			}
			catch(const std::exception &e) {
				g_SMAPI->ConPrintf("Error: %s\n", e.what());
			}

			return;
		}
		else if(subCommand == "unload") {
			if(argCount != 5) {
				g_SMAPI->ConPrint("Usage: sm py plugins unload <plugin path or #plugin ID>\n");
				return;
			}

			std::string unloadStr(commandArgs->Arg(4));

			if(unloadStr[0] == '#') {
				int pluginID = atoi(unloadStr.substr(1).c_str());

				try {
					g_ViperExtension.GetPluginManager()->UnloadPluginByID(pluginID);

					g_SMAPI->ConPrintf("Plugin #%d unloaded.\n", pluginID);
				}
				catch(const std::exception &e) {
					g_SMAPI->ConPrintf("Error: %s\n", e.what());
				}

				return;
			}

			char fullPath[PLATFORM_MAX_PATH];

			g_pSM->BuildPath(SourceMod::Path_SM, fullPath, sizeof(fullPath), "plugins/%s", unloadStr.c_str());

			std::string fullPathStr(fullPath);

			try {
				g_ViperExtension.GetPluginManager()->UnloadPluginByPath(fullPathStr);

				g_SMAPI->ConPrintf("Plugin \"%s\" unloaded.\n", unloadStr.c_str());
			}
			catch(const std::exception &e) {
				g_SMAPI->ConPrintf("Error: %s\n", e.what());
			}

			return;
		}
		else if(subCommand == "list") {
			int plCount = g_ViperExtension.GetPluginManager()->GetLoadedPluginCount();

			for(int plIndex = 1; plIndex <= plCount; plIndex++) {
				ViperPlugin *plugin = g_ViperExtension.GetPluginManager()->FindPluginByID(plIndex);

				g_SMAPI->ConPrintf("%d. %s\n", plIndex, plugin->GetPluginDirectory().c_str());
			}

			return;
		}

		// Unknown command.
		ListPluginCommands();
		return;
	}

	// Unknown command.
	ListAllCommands();
}

void ViperRootConsoleCommand::ListAllCommands() {
	g_SMAPI->ConPrint("Viper commands:\n");
	rootconsole->DrawGenericOption("plugins", "Viper plugin commands.");
}

void ViperRootConsoleCommand::ListPluginCommands() {
	g_SMAPI->ConPrint("Viper plugin commands:\n");
	rootconsole->DrawGenericOption("list", "List all running Viper plugins.");
	rootconsole->DrawGenericOption("load", "Load a Viper plugin.");
	rootconsole->DrawGenericOption("unload", "Unload a Viper plugin.");
}