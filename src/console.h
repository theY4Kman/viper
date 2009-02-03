/**
 * =============================================================================
 * Viper
 * Copyright (C) 2008-2009 Zach "theY4Kman" Kanzler
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

#ifndef _INCLUDE_VIPER_CONSOLE_H_
#define _INCLUDE_VIPER_CONSOLE_H_

#include "viper_globals.h"
#include <IRootConsoleMenu.h>
#include <convar.h>
#include <compat_wrappers.h>

class ViperConsole :
    public SourceMod::IRootConsoleCommand,
    public ViperGlobalClass
{
public: // IRootConsoleCommand
	// Called when an admin runs "sm py"
	virtual void OnRootConsoleCommand(char const *cmdname, const CCommand &command);

public: // ViperGlobalClass
	virtual void OnViperStartup(bool late);
};

#endif /* _INCLUDE_VIPER_CONSOLE_H_ */

