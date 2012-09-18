#include "BoostPythonSM.h"
#include "EntityOutputClassNameHook.h"
#include "sdk/smsdk_ext.h"
#include "Macros.h"

namespace py = boost::python;

EntityOutputClassNameHook::EntityOutputClassNameHook(std::string className, std::string output, PyThreadState *threadState, boost::python::object callbackFunction) {
	ClassName = className;
	Output = output;
	ThreadState = threadState;
	CallbackFunction = callbackFunction;
}