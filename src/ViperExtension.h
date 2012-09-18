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

#ifndef _INCLUDE_EXTENSION_H_
#define _INCLUDE_EXTENSION_H_

#include "HL2SDK.h"
#include "STL.h"
#include "BoostPythonSM.h"
#include "sdk/smsdk_ext.h"
#include "ViperPluginManager.h"
#include "Globals.h"
#include "IBinTools.h"

class ViperExtension : public SDKExtension {
public:
	ViperExtension();
	~ViperExtension();

	void InitializePython();
	void InitializePluginManager();
public:
	virtual bool SDK_OnLoad(char *error, size_t maxlength, bool late);
	virtual void SDK_OnUnload();
	virtual void SDK_OnAllLoaded();

public:
	virtual bool SDK_OnMetamodLoad(ISmmAPI *ismm, char *error, size_t maxlength, bool late);
	virtual void OnCoreMapStart(edict_t *pEdictList, int edictCount, int clientMax);

public:
	void OnGameFrame(bool simulating);
	void OnLevelShutdown();

#if SOURCE_ENGINE >= SE_ORANGEBOX
	void OnClientCommand(edict_t *edict, const CCommand &command);
#else
	void OnClientCommand(edict_t *edict);
#endif

	ViperPluginManager *GetPluginManager();

private:
	// This is available after InitializePython()
	std::string PythonHome;

	// This is available after InitializePluginManager()
	std::string PluginsDirectory;

	// Also available after InitializePluginManager()
	ViperPluginManager *PluginManagerInstance;
};

extern ViperExtension g_ViperExtension;

#endif /* _INCLUDE_EXTENSION_H_ */