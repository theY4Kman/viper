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

#ifndef _INCLUDE_VIPER_CONSOLE_H_
#define _INCLUDE_VIPER_CONSOLE_H_

#include "viper_globals.h"
#include <IRootConsoleMenu.h>
#include <compat_wrappers.h>

class ViperConsole :
    public SourceMod::IRootConsoleCommand,
    public ViperGlobalClass,
    public IConCommandBaseAccessor,
    public ICommandCallback
{
public: // IRootConsoleCommand
	// Called when an admin runs "sm py"
	virtual void OnRootConsoleCommand(char const *cmdname, const CCommand &command);
public: // ViperGlobalClass
	virtual void OnViperStartup(bool late);
	virtual void OnViperAllShutdown();
public: // IConCommandBaseAccessor
    bool RegisterConCommandBase(ConCommandBase *pCommand);
public: // ICommandCallbackvoid
    void CommandCallback(const CCommand &command);

public:
    ViperConsole::ViperConsole();

private:
    ConCommandBase *m_InterpCmd;
    PyThreadState *m_pThreadState;
    PyInterpreterState *m_pInterpState;
    PyObject *m_InterpGlobals;
};

extern ViperConsole g_VConsole;

#endif /* _INCLUDE_VIPER_CONSOLE_H_ */

