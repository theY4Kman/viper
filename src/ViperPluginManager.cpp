#include "ViperPluginManager.h"
#include "sdk/smsdk_ext.h"
#include "Util.h"
#include <boost\filesystem.hpp>

ViperPluginManager::ViperPluginManager(std::string pythonHome) {
	PythonHome = pythonHome;
	Plugins = new SourceHook::List<ViperPlugin*>();
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
}

ViperPlugin *ViperPluginManager::FindPluginByInitPluginPath(
	std::string initPluginPath) {
	for(SourceHook::List<ViperPlugin*>::iterator it = Plugins->begin(); 
		it != Plugins->end(); it++) {
		ViperPlugin *Plugin = *it;

		if(Plugin->GetInitPluginPath() != initPluginPath) {
			continue;
		}

		return Plugin;
	}

	return NULL;
}

ViperPlugin *ViperPluginManager::FindPluginByDirectory(
	std::string pluginDirectory) {
	for(SourceHook::List<ViperPlugin*>::iterator it = Plugins->begin(); 
		it != Plugins->end(); it++) {
		ViperPlugin *plugin = *it;

		if(plugin->GetPluginDirectory() != pluginDirectory) {
			continue;
		}

		return plugin;
	}
	
	return NULL;
}