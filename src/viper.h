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

#ifndef _INCLUDE_VIPER_VIPER_H_
#define _INCLUDE_VIPER_VIPER_H_

#include "IViper.h"
#include <sh_list.h>
#include <sh_stack.h>
#include "ForwardSys.h"
#include <IViperForwardSys.h>

struct CachedCommandInfo
{
	const CCommand *args;
#if !defined ORANGEBOX_BUILD
	char cmd[300];
#endif
};

class BaseViper : public IBaseViper
{
public: // IBaseViper
    virtual void PushCommandStack(const CCommand *cmd);
    virtual const CCommand *PeekCommandStack();
    virtual void PopCommandStack();

public:
    /**
     * @brief   Called when the extension is loaded; initializes Viper
     */
    bool OnViperLoad(char *error, size_t maxlength, bool late);
    
    /**
     * @brief   Called when the extension is unloaded
     */
    void OnViperUnload();
    
    /**
     * @brief   Called after Viper has been initialized and begins running it
     */
    void StartViper();
    
    /**
     * @brief   Called on each game frame
     */
    void GameFrame(bool simulating);
    
private:
	SourceHook::CStack<CachedCommandInfo> m_CommandStack;
	
	IViperForward *m_GameFrame;
	PyObject *m_EmptyTuple;
};

extern BaseViper g_Viper;

#endif /* _INCLUDE_VIPER_VIPER_H_ */
