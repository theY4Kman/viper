#ifndef __INCLUDE_CONSOLECLIENTVARIABLEQUERYINFO_H__
#define __INCLUDE_CONSOLECLIENTVARIABLEQUERYINFO_H__

#include "STL.h"
#include "HL2SDK.h"
#include "BoostPythonSM.h"
#include <public\engine\iserverplugin.h>

class ConsoleClientVariableQueryInfo {
public:
	ConsoleClientVariableQueryInfo(QueryCvarCookie_t cookie, PyThreadState *threadState, boost::python::object callbackFunction);

	QueryCvarCookie_t Cookie;
	PyThreadState *ThreadState;
	boost::python::object CallbackFunction;
};

#endif