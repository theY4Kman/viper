#include "ViperPluginManager.h"
#include "sdk/smsdk_ext.h"
#include "Util.h"
#include <boost\filesystem.hpp>

ViperPluginManager::ViperPluginManager(std::string pythonHome) {
	PythonHome = pythonHome;
}

void ViperPluginManager::LoadPluginsInDirectory(std::string pluginsDirectory) {
	char error[256];
	SourceMod::IDirectory *directoryInstance = libsys->OpenDirectory(pluginsDirectory.c_str());
	
	if(directoryInstance == NULL) {
		libsys->GetPlatformError(error, sizeof(error));

		g_pSM->LogError(myself, "Could not load plugins from %s", pluginsDirectory);
		g_pSM->LogError(myself, "Platform returned error: %s", error);

		return;
	}

	char initPluginPath[PLATFORM_MAX_PATH];

	while(directoryInstance->MoreFiles()) {
		const char *directoryEntryName = directoryInstance->GetEntryName();
		if (directoryInstance->IsEntryDirectory()
			&& strcmp(directoryEntryName, ".") != 0
			&& strcmp(directoryEntryName, "..") != 0
			&& strcmp(directoryEntryName, "disabled") != 0
			&& strcmp(directoryEntryName, "optional") != 0) {
			UTIL_Format(initPluginPath, sizeof(initPluginPath),
				"%s/%s/__init__.py", pluginsDirectory.c_str(), directoryEntryName);

			if (libsys->PathExists(initPluginPath)) {
				LoadPlugin(initPluginPath);
			}
		}

		directoryInstance->NextEntry();
	}

	libsys->CloseDirectory(directoryInstance);
}

void ViperPluginManager::LoadPlugin(std::string initPluginPath) {
	ViperPlugin *plugin = FindPluginByInitPluginPath(initPluginPath);

	boost::filesystem::path fileSystemPath(initPluginPath);

	if(plugin != NULL) {
		boost::throw_exception(std::exception((
			std::string("Plugin already loaded: ") +
			std::string(initPluginPath)).c_str()));
	}

	plugin = new ViperPlugin(initPluginPath, PythonHome);
	plugin->Run();

	LoadedPlugins.push_back(plugin);
}

void ViperPluginManager::UnloadPlugin(ViperPlugin *plugin) {
	plugin->Unload();

	for(std::vector<ViperPlugin*>::iterator it = LoadedPlugins.begin();
		it != LoadedPlugins.end(); it++) {
		ViperPlugin *otherPlugin = *it;

		if(plugin != otherPlugin) {
			continue;
		}

		LoadedPlugins.erase(it);
		break;
	}

	delete plugin;
}

void ViperPluginManager::UnloadPluginByPath(std::string pluginPath) {
	ViperPlugin *plugin = FindPluginByInitPluginPath(pluginPath);

	if(plugin != NULL) {
		UnloadPlugin(plugin);
		return;
	}

	plugin = FindPluginByDirectory(pluginPath);

	if(plugin != NULL) {
		UnloadPlugin(plugin);
		return;
	}

	char str[1024];
	UTIL_Format(str, sizeof(str), "Plugin not loaded: %s", pluginPath.c_str());

	boost::throw_exception(std::exception(str));
}

void ViperPluginManager::UnloadPluginByID(int pluginID) {
	if(pluginID > LoadedPlugins.size()) {
		char str[256];
		UTIL_Format(str, sizeof(str), "Plugin not loaded: #%d", pluginID);

		boost::throw_exception(std::exception(str));
	}
	
	int pluginIndex = pluginID - 1;

	ViperPlugin *plugin = LoadedPlugins[pluginIndex];

	UnloadPlugin(plugin);
}

ViperPlugin *ViperPluginManager::FindPluginByID(int pluginID) {
	if(pluginID < 1 || pluginID > LoadedPlugins.size()) {
		return NULL;
	}

	return LoadedPlugins[pluginID - 1];
}

ViperPlugin *ViperPluginManager::FindPluginByInitPluginPath(
	std::string initPluginPath) {
	for(std::vector<ViperPlugin*>::iterator it = LoadedPlugins.begin(); 
		it != LoadedPlugins.end(); it++) {
		ViperPlugin *Plugin = *it;

		if(boost::filesystem::path(Plugin->GetInitPluginPath()) != boost::filesystem::path(initPluginPath)) {
			continue;
		}

		return Plugin;
	}

	return NULL;
}

ViperPlugin *ViperPluginManager::FindPluginByDirectory(
	std::string pluginDirectory) {
	for(std::vector<ViperPlugin*>::iterator it = LoadedPlugins.begin(); 
		it != LoadedPlugins.end(); it++) {
		ViperPlugin *plugin = *it;

		if(boost::filesystem::path(plugin->GetPluginDirectory()) != boost::filesystem::path(pluginDirectory)) {
			continue;
		}

		return plugin;
	}
	
	return NULL;
}

int ViperPluginManager::GetLoadedPluginCount() {
	return LoadedPlugins.size();
}