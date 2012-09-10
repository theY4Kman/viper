#include "BoostPythonSM.h"
#include "ConsoleCommandHookInfo.h"
#include "sdk/smsdk_ext.h"
#include "Macros.h"

namespace py = boost::python;

ConsoleCommandHookInfo::ConsoleCommandHookInfo(std::string name, PyThreadState *threadState, py::object callbackFunction) {
	Name = name;
	ThreadState = threadState;
	CallbackFunction = callbackFunction;
}