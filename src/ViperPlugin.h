#ifndef __INCLUDE_PLUGIN_H__
#define __INCLUDE_PLUGIN_H__

#include "STL.h"
#include <boost/python.hpp>
#include <Python.h>

class ViperPlugin {
public:
	ViperPlugin(std::string initPluginPath, std::string pythonHome);
	~ViperPlugin();

	void Run();
	void Unload();

	std::string GetInitPluginPath();
	std::string GetPluginDirectory();

private:
	// Sub-interpreter thread state
	PyThreadState *ThreadState;

	// Init plugin path
	std::string InitPluginPath;

	// Plugin directory
	std::string PluginDirectory;

	// Python home directory
	std::string PythonHome;

	// Python lib directory
	std::string PythonLibDirectory;
};

#endif