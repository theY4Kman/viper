#ifndef __INCLUDE_PLUGINMANAGER_H__
#define __INCLUDE_PLUGINMANAGER_H__

#include "STL.h"
#include <sh_list.h>
#include "Plugin.h"

namespace Viper {
	class PluginManager {
	public:
		PluginManager(std::string pythonHome);

		void LoadPluginsInDirectory(std::string pluginsDirectory);
		void LoadPlugin(std::string initPluginPath);

		Plugin *FindPluginByInitPluginPath(std::string initPluginPath);
		Plugin *FindPluginByDirectory(std::string initPluginPath);

	private:
		std::string PythonHome;
		SourceHook::List<Plugin*> *Plugins;
	};
}

#endif