#include "TempEntHook.h"

namespace py = boost::python;

TempEntHook::TempEntHook(std::string effectName, py::object pythonCallback, PyThreadState *threadState) {
	EffectName = effectName;
	PythonCallback = pythonCallback;
	ThreadState = threadState;
}