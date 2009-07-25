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

#ifndef _INCLUDE_VIPER_HALFLIFE2_H_
#define _INCLUDE_VIPER_HALFLIFE2_H_

#include "extension.h"
#include "viper_globals.h"

class KeyValues;

class ViperHalfLife2 : public ViperGlobalClass
{
public: // ViperGlobalClass
    virtual void OnViperStartup(bool late);
    virtual void OnViperAllInitialized();
public:
    bool HintTextMsg(cell_t clients[], char const *msg);
    inline bool HintTextMsg(int client, char const *msg)
    {
        cell_t players[] = {client};
        return HintTextMsg(players, msg);
    }
    
    bool ShowVGUIMenu(cell_t clients[], char const *name, KeyValues *data, bool show);
    inline bool ShowVGUIMenu(int client, char const *name, KeyValues *data, bool show)
    {
        cell_t players[] = {client};
        return ShowVGUIMenu(players, name, data, show);
    }

private:
    int m_HintTextMsg;
    int m_VGUIMenu;
};

extern ViperHalfLife2 g_HL2;

#endif//_INCLUDE_VIPER_HALFLIFE2_H_

