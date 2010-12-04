/**
 * =============================================================================
 * Viper
 * Copyright (C) 2007-2010 Zach "theY4Kman" Kanzler
 * Copyright (C) 2004-2007 AlliedModders LLC.
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

ViperConsole::ViperConsole()
{
    m_InterpCmd = NULL;
    m_InterpGlobals = NULL;
}

void
ViperConsole::OnViperStartup(bool late)
{
    g_pMenu->AddRootConsoleCommand("py", "Viper (Python) menu", this);
}

void
ViperConsole::OnViperAllShutdown()
{
    g_pMenu->RemoveRootConsoleCommand("py", this);
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
		g_pMenu->ConsolePrint("    Built %s %s", __DATE__, __TIME__);
		g_pMenu->ConsolePrint("    http://y4kstudios.com/viper/");
		
		delete [] py_ver;
		return;
	}
	else if (strcmp(cmd, "setinterp") == 0)
	{
	    char const *cmdname = NULL;
	    if (command.ArgC() >= 3)
	        cmdname = command.Arg(3);
	    else
	    {
	        g_pMenu->ConsolePrint("[Viper] If an interpreter command has not been set, "
	            "provide one. If one has been set, provide the current command name "
	            "to clear it.");
	        return;
	    }
	    
	    if (m_InterpCmd != NULL)
	    {
	        if (command.ArgC() >= 3)
	        {
	            if (strcmp(cmdname, m_InterpCmd->GetName()) == 0)
	            {
	                g_pMenu->ConsolePrint("[Viper] Interpreter command cleared.");
	                META_UNREGCVAR(m_InterpCmd);
	                m_InterpCmd = NULL;
	                
	                return;
	            }
	        }
	        
            g_pMenu->ConsolePrint("[Viper] Interpreter command already set. Supply the"
                " correct current interpreter command to clear it.");
            return;
	    }
	    
	    ConCmdInfo *pInfo = g_VCmds.AddOrFindCommand(cmdname, "",
	        FCVAR_PROTECTED|FCVAR_HIDDEN);
	    m_InterpCmd = pInfo->pCmd;
	    
	    g_pMenu->ConsolePrint("[Viper] Interpreter command set successfully!");
	    
	    return;
	}

	g_pMenu->ConsolePrint("Viper menu:");
	g_pMenu->DrawGenericOption("cmds", "View commands created by a plugin");
	g_pMenu->DrawGenericOption("credits", "Display credits listing");
	g_pMenu->DrawGenericOption("cvars", "View convars created by a plugin");
	g_pMenu->DrawGenericOption("plugins", "Manage plugins");
	g_pMenu->DrawGenericOption("version", "Display version information");
}

void
ViperConsole::CommandCallback(const CCommand &command)
{
    if (m_InterpCmd == NULL || strcmp(command.Arg(0),
        m_InterpCmd->GetName()) != 0)
        return;
    
    char const *exec = NULL;
    PyThreadState *tstate = NULL;
    PyObject *globals = NULL;
    
    /* interpcmd plugin_id execcmd */
    if (command.ArgC() == 3)
    {
        char const *pl_id = command.Arg(1);
        exec = command.Arg(2);
        
        IViperPlugin *pl = g_VPlugins.FindPluginByConsoleArg(pl_id);
        if (pl == NULL)
        {
            g_SMAPI->ConPrintf("[Viper] Could not locate plugin \"%s\"\n", pl_id);
            return;
        }
        
        tstate = pl->GetThreadState();
        globals = pl->GetPluginDict();
    }
    
    /* interpcmd execcmd */
    else if (command.ArgC() == 2)
    {
        if (m_pThreadState == NULL)
        {
            /* Create a completeley new environment (sub-interpreter) */
            m_pThreadState = Py_NewInterpreter();
            if (m_pThreadState == NULL)
            {
                g_pSM->LogError(myself, "Fatal error in creating Python "
                    "sub-interpreter for interp cmd!");
                return;
            }
            
            m_pInterpState = m_pThreadState->interp;
        }
        
        tstate = m_pThreadState;
        exec = command.Arg(1);
        
        if (m_InterpGlobals == NULL)
        {
            PyThreadState *_swap = PyThreadState_Get();
            PyThreadState_Swap(tstate);
            
            PyObject *__main__ = PyImport_ImportModule("__main__");
            m_InterpGlobals = PyModule_GetDict(__main__);
            Py_DECREF(__main__);
            
            PyThreadState_Swap(_swap);
        }
        
        globals = m_InterpGlobals;
    }
    
    else
    {
        g_SMAPI->ConPrint("[Viper] Syntax: interpcmd [plugin_id] <execcmd>\n");
        return;
    }
    
    
    PyThreadState *_swap = PyThreadState_Get();
    PyThreadState_Swap(tstate);
    
    if (globals == NULL)
        globals = PyEval_GetGlobals();
    
    PyObject *result = PyRun_String(exec, Py_single_input, globals, NULL);
    
    if (result == NULL)
        PyErr_Print();
    else if (result != Py_None)
    {
        PyObject *py_str = result->ob_type->tp_repr(result);
        char const *str = PyString_AS_STRING(py_str);
        
        g_SMAPI->ConPrintf("[Viper] %s\n", str);
        Py_DECREF(py_str);
    }
    
    PyThreadState_Swap(_swap);
}

bool
ViperConsole::RegisterConCommandBase(ConCommandBase *pCommand)
{
    META_REGCVAR(pCommand);
    
    return true;
}

ViperConsole g_VConsole;

