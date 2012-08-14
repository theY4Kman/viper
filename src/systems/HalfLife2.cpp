/**
 * =============================================================================
 * Viper
 * Copyright (C) 2012 PimpinJuice
 * Copyright (C) 2007-2012 Zach "theY4Kman" Kanzler
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

#include "HalfLife2.h"
#include <IGameConfigs.h>

using namespace Viper;

ViperHalfLife2 g_HL2;

SourceMod::IGameConfig *g_pGameConf = NULL;

namespace Viper {
	void
	ViperHalfLife2::OnViperStartup(bool late)
	{
		gameconfs->LoadGameConfigFile("core.games", &g_pGameConf, NULL, 0);
	}

	void
	ViperHalfLife2::OnViperAllInitialized()
	{
		m_HintTextMsg = usermsgs->GetMessageIndex("HintText");
		m_VGUIMenu = usermsgs->GetMessageIndex("VGUIMenu");
	}

	bool
	ViperHalfLife2::HintTextMsg(cell_t clients[], char const *msg)
	{
		bf_write *pBitBuf = usermsgs->StartMessage(m_HintTextMsg, clients, 1,
			USERMSG_RELIABLE);
    
		if (pBitBuf == NULL)
			return false;
    
		char const *pre_byte = g_pGameConf->GetKeyValue("HintTextPreByte");
		if (pre_byte != NULL && strcmp(pre_byte, "yes") == 0)
			pBitBuf->WriteByte(1);
    
		pBitBuf->WriteString(msg);
		usermsgs->EndMessage();
    
		return true;
	}

	bool
	ViperHalfLife2::ShowVGUIMenu(cell_t clients[], char const *name,
								 KeyValues *data, bool show)
	{
		bf_write *pBitBuf = usermsgs->StartMessage(m_VGUIMenu, clients, 1,
			USERMSG_RELIABLE);
    
		if (pBitBuf == NULL)
			return false;
    
		KeyValues *SubKey = NULL;
		size_t count = 0;
		if (data)
		{
			SubKey = data->GetFirstSubKey();
			while (SubKey)
			{
				count++;
				SubKey = SubKey->GetNextKey();
			}
			SubKey = data->GetFirstSubKey();
		}
    
		pBitBuf->WriteString(name);
		pBitBuf->WriteByte((show) ? 1 : 0);
		pBitBuf->WriteByte(count);
    
		while (SubKey)
		{
			pBitBuf->WriteString(SubKey->GetName());
			pBitBuf->WriteString(SubKey->GetString());
			SubKey = SubKey->GetNextKey();
		}
    
		usermsgs->EndMessage();
		return true;
	}
}