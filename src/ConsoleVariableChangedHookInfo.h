#ifndef __INCLUDE_CONSOLEVARIABLECHANGEDHOOKINFO_H__
#define __INCLUDE_CONSOLEVARIABLECHANGEDHOOKINFO_H__

#include "Macros.h"
#include "HL2SDK.h"

class ConsoleVariableChangedHookInfo {
public:
	ConsoleVariableChangedHookInfo(std::string name, PyThreadState *threadState, boost::python::object callbackFunction);

	std::string Name;
	PyThreadState *ThreadState;
	boost::python::object CallbackFunction;
};

#endif