#include "ConsoleClientVariableQueryInfo.h"

namespace py = boost::python;

ConsoleClientVariableQueryInfo::ConsoleClientVariableQueryInfo(QueryCvarCookie_t cookie, PyThreadState *threadState, py::object callbackFunction) {
	Cookie = cookie;
	ThreadState = threadState;
	CallbackFunction = callbackFunction;
}