#include "BoostPythonSM.h"
#include "ConsoleVariableChangedHookInfo.h"

namespace py = boost::python;

ConsoleVariableChangedHookInfo::ConsoleVariableChangedHookInfo(std::string name, PyThreadState *threadState, py::object callbackFunction) {
	Name = name;
	ThreadState = threadState;
	CallbackFunction = callbackFunction;
}