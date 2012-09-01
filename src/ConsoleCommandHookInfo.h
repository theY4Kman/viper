#ifndef __INCLUDE_CONSOLECOMMANDHOOKINFO_H__
#define __INCLUDE_CONSOLECOMMANDHOOKINFO_H__

#include "Macros.h"
#include "HL2SDK.h"

class ConsoleCommandHookInfo {
public:
	ConsoleCommandHookInfo(std::string name, PyThreadState *threadState, boost::python::object callbackFunction);

	std::string Name;
	PyThreadState *ThreadState;
	boost::python::object CallbackFunction;
};

#endif