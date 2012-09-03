#include "ViperPlugin.h"
#include <boost/exception/all.hpp>
#include <boost/filesystem.hpp>
#include "ViperExtension.h"
#include "SysHooks.h"
#include "ForwardsModule.h"
#include "SourcemodModule.h"
#include "BitBufModule.h"
#include "HalflifeModule.h"
#include "DatatypesModule.h"
#include "EntityModule.h"
#include "SysHooks.h"
#include "ForwardsModule.h"
#include "ClientsModule.h"
#include "EventsModule.h"
#include "UserMessagesModule.h"
#include "ConsoleModule.h"

namespace py = boost::python;

ViperPlugin::ViperPlugin(std::string initPluginPath, std::string pythonHome) {
	PythonHome = pythonHome;
	PythonLibDirectory = boost::filesystem::path(PythonHome + "/lib").string();

	InitPluginPath = initPluginPath;

	boost::filesystem::path fileSystemPath(InitPluginPath);
	PluginDirectory = fileSystemPath.parent_path().string();
	
	ThreadState = NULL;
}

ViperPlugin::~ViperPlugin() {
	// We need to set the current thread state before we can destroy the state.

	/**
		* Py_EndInterpreter()
		* Destroy the (sub-)interpreter represented by the given thread state.
		* The given thread state must be the current thread state.
		* See the discussion of thread states below. When the call returns,
		* the current thread state is NULL. All thread states associated with this
		* interpreter are destroyed. (The global interpreter lock must be held
		* before calling this function and is still held when it returns.)
		* Py_Finalize() will destroy all sub-interpreters that haven’t been
		* explicitly destroyed at that point.
		*/
	if(ThreadState) {
		PyThreadState *currentThreadState = PyThreadState_Get();

		PyThreadState_Swap(ThreadState);

		Py_EndInterpreter(ThreadState);

		PyThreadState_Swap(currentThreadState);
	}
}

std::string ViperPlugin::GetInitPluginPath() {
	return InitPluginPath;
}

void ViperPlugin::Run() {
	PyThreadState *oldThreadState = PyThreadState_Get();

	ThreadState = Py_NewInterpreter();

	if(ThreadState == NULL) {
		boost::throw_exception(
			std::exception("Failed to create sub-interpreter")
		);
	}

	boost::filesystem::path fileSystemPath(InitPluginPath);

	py::list pathList;
	pathList.append<std::string>(fileSystemPath.parent_path().string());
	pathList.append<std::string>(PythonLibDirectory);
	pathList.append<std::string>(PythonLibDirectory + "site-packages/");

	py::object sysModule = py::import("sys");
	sysModule.attr("path") = pathList;

	py::object tracebackModule = py::import("traceback");
	py::object printException = tracebackModule.attr("print_exception");
	
	sysModule.attr("excepthook") = printException;

	/* Create the plug-in's globals dict */
	py::object mainModule = py::import("__main__");
	
	py::object mainDict = mainModule.attr("__dict__");
//	pluginDict.attr("__file__") = InitPluginPath;

	initsyshooks(mainDict, sysModule);

	try {
		py::import("Sourcemod");
		py::import("BitBuf");
		py::import("Halflife");
		py::import("Datatypes");
		py::import("Entity");
		py::import("Forwards");
		py::import("Clients");
		py::import("Events");
		py::import("UserMessages");
		py::import("Console");

		py::exec_file(InitPluginPath.c_str(), mainDict, mainDict);
	}
	catch(const py::error_already_set &) {
		PyErr_Print();
	}
	
	PyThreadState_Swap(oldThreadState);
}

void ViperPlugin::Unload() {
	if(!ThreadState) {
		return;
	}

	unloadThreadStateSourcemod(ThreadState);
	unloadThreadStateBitBuf(ThreadState);
	unloadThreadStateHalflife(ThreadState);
	unloadThreadStateDatatypes(ThreadState);
	unloadThreadStateEntity(ThreadState);
	unloadThreadStateForwards(ThreadState);
	unloadThreadStateClients(ThreadState);
	unloadThreadStateEvents(ThreadState);
	unloadThreadStateUserMessages(ThreadState);
	unloadThreadStateConsole(ThreadState);
}

std::string ViperPlugin::GetPluginDirectory() {
	return PluginDirectory;
}