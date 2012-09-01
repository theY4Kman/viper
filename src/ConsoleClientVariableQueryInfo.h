#ifndef __INCLUDE_CONSOLECLIENTVARIABLEQUERYINFO_H__
#define __INCLUDE_CONSOLECLIENTVARIABLEQUERYINFO_H__

#include "Macros.h"
#include "HL2SDK.h"

class ConsoleClientVariableQueryInfo {
public:
	ConsoleClientVariableQueryInfo(QueryCvarCookie_t cookie, PyThreadState *threadState, boost::python::object callbackFunction);

	QueryCvarCookie_t Cookie;
	PyThreadState *ThreadState;
	boost::python::object CallbackFunction;
};

#endif