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

	ViperPlugin *FindPluginByInitPluginPath(std::string initPluginPath);
	ViperPlugin *FindPluginByDirectory(std::string initPluginPath);

private:
	std::string PythonHome;
	SourceHook::List<ViperPlugin*> *Plugins;
};

#endif