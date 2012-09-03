#include "EventHook.h"
#include "Macros.h"
#include "BoostPythonSM.h"
#include "Util.h"

namespace py = boost::python;

EventHook::EventHook(std::string eventName, py::object pythonCallback, PyThreadState *threadState, EventHookModes hookMode) {
	EventName = eventName;
	PythonCallback = pythonCallback;
	ThreadState = threadState;
	HookMode = hookMode;
}