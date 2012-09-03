#ifndef __INCLUDE_PLUGINMANAGER_H__
#define __INCLUDE_PLUGINMANAGER_H__

#include "STL.h"
#include <sh_list.h>
#include "ViperPlugin.h"

class ViperPluginManager {
public:
	ViperPluginManager(std::string pythonHome);

	void LoadPluginsInDirectory(std::string pluginsDirectory);
	void LoadPlugin(std::string initPluginPath);

	
	void UnloadPluginByID(int pluginID);
	void UnloadPluginByPath(std::string pluginPath);
	void UnloadPlugin(ViperPlugin *plugin);

	ViperPlugin *FindPluginByInitPluginPath(std::string initPluginPath);
	ViperPlugin *FindPluginByDirectory(std::string initPluginPath);
	ViperPlugin *FindPluginByID(int pluginID);

	int GetLoadedPluginCount();

private:
	std::string PythonHome;
	std::vector<ViperPlugin*> LoadedPlugins;
};

#endif