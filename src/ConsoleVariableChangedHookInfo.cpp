#include "BoostPythonSM.h"
#include "ConsoleVariableChangedHookInfo.h"
#include "sdk/smsdk_ext.h"
#include "Macros.h"
#include "ViperExtension.h"
#include "CompatWrappers.h"

namespace py = boost::python;

ConsoleVariableChangedHookInfo::ConsoleVariableChangedHookInfo(std::string name, PyThreadState *threadState, py::object callbackFunction) {
	Name = name;
	ThreadState = threadState;
	CallbackFunction = callbackFunction;
}