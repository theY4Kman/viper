#include "ForwardType.h"
#include "Macros.h"
#include "BoostPythonSM.h"
#include "Util.h"
#include "ForwardFunction.h"

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
	for(std::list<ForwardFunction>::iterator it = Functions.begin();
		it != Functions.end(); it++) {
		ForwardFunction fwdFunction = *it;

		PyThreadState *oldThreadState = PyThreadState_Get();

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
	for(std::list<ForwardFunction>::iterator it = Functions.begin();
		it != Functions.end(); it++) {
		ForwardFunction otherFn = *it;

		if(fn != otherFn.PythonFunction) {
			continue;
		}
		
		Functions.erase(it);
		return true;
	}

	return false;
}

bool ForwardType::RemoveFirstFunctionByThreadState(PyThreadState *threadState) {
	for(std::list<ForwardFunction>::iterator it = Functions.begin();
		it != Functions.end(); it++) {
		ForwardFunction fn = *it;

		if(fn.ThreadState != threadState) {
			continue;
		}

		it = Functions.erase(it);
		return true;
	}

	return false;
}

void ForwardType::RemoveFunctionsByThreadState(PyThreadState *threadState) {
	bool searchAgain = true;
	
	while(searchAgain) {
		searchAgain = RemoveFirstFunctionByThreadState(threadState);
	}
}