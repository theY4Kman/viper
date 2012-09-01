#include "ForwardType.h"
#include "Macros.h"
#include "BoostPythonSM.h"
#include "Util.h"

namespace py = boost::python;

ForwardType::ForwardType(ForwardCallback callback, std::string forwardName) {
	Callback = callback;
	Name = forwardName;
}

void ForwardType::AddFunction(py::object fn) {
	ForwardFunction fwdFunction;

	fwdFunction.PythonFunction = fn;
	fwdFunction.ThreadState = PyThreadState_Get();

	Functions.push_back(fwdFunction);
}

int ForwardType::GetFunctionCount() {
	return Functions.size();
}

void ForwardType::Fire(py::list argumentsList) {
	int fnCount = GetFunctionCount();

	for(int x = 0; x < fnCount; x++) {
		PyThreadState *oldThreadState = PyThreadState_Get();

		ForwardFunction fwdFunction = Functions[x];

		PyThreadState_Swap(fwdFunction.ThreadState);

		py::object returnValue = py::object(true);

		try {
			py::object returnValue = fwdFunction.PythonFunction(*py::tuple(argumentsList));
		}
		catch(const py::error_already_set &) {
			PyErr_Print();
		}

		PyThreadState_Swap(oldThreadState);

		if(!Callback(returnValue)) {
			PyThreadState_Swap(oldThreadState);
			return;
		}
	}
}

bool ForwardType::RemoveFunction(py::object fn) {
	int fnCount = GetFunctionCount();

	for(int x = 0; x < fnCount; x++) {
		py::object otherFn = Functions[x].PythonFunction;

		if(fn != otherFn) {
			continue;
		}
		
		Functions.erase(Functions.begin() + x);
		return true;
	}

	return false;
}