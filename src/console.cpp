/**
 * =============================================================================
 * Viper
 * Copyright (C) 2007-2010 Zach "theY4Kman" Kanzler
 * Copyright (C) 2004-2007 AlliedModders LLC.  All rights reserved.
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "console.h"
#include "systems/ConCmdManager.h"
#include "systems/ConVarManager.h"
#include "systems/PluginSys.h"

ConVar Py_Version("viper_version", SMEXT_CONF_VERSION,
#if SOURCE_ENGINE >= SE_ORANGEBOX
    FCVAR_NOTIFY|FCVAR_REPLICATED,
#else
    FCVAR_PLUGIN|FCVAR_NOTIFY|FCVAR_REPLICATED,
#endif
    "The version of Viper installed.");


void
ViperConsole::OnViperStartup(bool late)
{
    g_pMenu->AddRootConsoleCommand("py", "Viper (Python) menu", this);
}

void
ViperConsole::OnRootConsoleCommand(char const *cmdname, const CCommand &command)
{
	char const *cmd = command.Arg(2);

	if (strcmp(cmd, "plugins") == 0)
	{
		g_VPlugins.OnRootConsoleCommand(cmd, command);
		return;
	}
	else if (strcmp(cmd, "cvars") == 0)
	{
		g_ConVarManager.OnRootConsoleCommand(cmd, command);
		return;
	}
	else if (strcmp(cmd, "cmds") == 0)
	{
		g_VCmds.OnRootConsoleCommand(cmd, command);
		return;
	}
	else if (strcmp(cmd, "credits") == 0)
	{
		g_pMenu->ConsolePrint(" Viper is developed by Zach \"theY4Kman\" Kanzler");
		g_pMenu->ConsolePrint(" Viper is an extension of SourceMod, developed by AlliedModders, LLC.");
		g_pMenu->ConsolePrint(" Viper's development would not have been possible without:");
		g_pMenu->ConsolePrint("  David \"BAILOPAN\" Anderson, for immense amounts of help");
		g_pMenu->ConsolePrint("  Matt \"pRED\" Woodrow, for lots of help and support");
		g_pMenu->ConsolePrint("  Mattie Casper, for invaluable help");
		g_pMenu->ConsolePrint("  CShadowRun, for the motivation");
		g_pMenu->ConsolePrint("  Tsunami, for always being there and enormous help with sigscanning.");
		g_pMenu->ConsolePrint(" A special shoutout to Viper from GameConnect: this is named after him.");
		g_pMenu->ConsolePrint(" Finally, I'd like to thank all the people of gs#sourcemod");
		g_pMenu->ConsolePrint(" http://y4kstudios.com/viper/");
		return;
	}
	else if (strcmp(cmd, "version") == 0)
	{
		/* Get the full Python version string, then cut it to only the
		 * version number (eg, 2.5.1)
		 */
		char *py_ver = sm_strdup(Py_GetVersion());
		*strchr(py_ver, ' ') = '\0';

		g_pMenu->ConsolePrint(" Viper Version Information:");
		g_pMenu->ConsolePrint("    Viper Version: %s", SMEXT_CONF_VERSION);
		g_pMenu->ConsolePrint("    Python Version: %s (Linked against %s)", py_ver, PY_VERSION);
		g_pMenu->ConsolePrint("    http://y4kstudios.com/viper/");
		
		delete [] py_ver;
		return;
	}

	g_pMenu->ConsolePrint("Viper menu:");
	g_pMenu->DrawGenericOption("cmds", "View commands created by a plugin");
	g_pMenu->DrawGenericOption("credits", "Display credits listing");
	g_pMenu->DrawGenericOption("cvars", "View convars created by a plugin");
	g_pMenu->DrawGenericOption("plugins", "Manage plugins");
	g_pMenu->DrawGenericOption("version", "Display version information");
}

bool
ViperConsole::RegisterConCommandBase(ConCommandBase *pCommand)
{
    META_REGCVAR(pCommand);
    
    return true;
}

ViperConsole g_VConsole;

