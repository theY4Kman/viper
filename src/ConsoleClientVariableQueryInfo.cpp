#include "BoostPythonSM.h"
#include "ConsoleClientVariableQueryInfo.h"
#include "sdk/smsdk_ext.h"
#include "Macros.h"
#include "ViperExtension.h"
#include "CompatWrappers.h"

namespace py = boost::python;

ConsoleClientVariableQueryInfo::ConsoleClientVariableQueryInfo(QueryCvarCookie_t cookie, PyThreadState *threadState, py::object callbackFunction) {
	Cookie = cookie;
	ThreadState = threadState;
	CallbackFunction = callbackFunction;
}