#include "TimeoutInfo.h"
#include "TimersModule.h"
#include "InterfaceContainer.h"

namespace py = boost::python;

TimeoutInfo::TimeoutInfo(float time, float timeout, PyThreadState *threadState, py::object callbackFunction) {
	Time = time;
	Timeout = timeout;
	ThreadState = threadState;
	CallbackFunction = callbackFunction;
}

void TimeoutInfo::Execute() {
	PyThreadState *oldThreadState = PyThreadState_Get();

	PyThreadState_Swap(ThreadState);

	try {
		CallbackFunction();
	}
	catch(const py::error_already_set &) {
		PyErr_Print();
	}

	PyThreadState_Swap(oldThreadState);

	// hack: cancel it
	Cancel();
}

void TimeoutInfo::Reset() {
	Time = timers__ActualTime + Timeout;
}

void TimeoutInfo::Cancel() {
	if(!timers__IsTimeoutValid(this)) { // just incase people cancel a timeout in their callback, then we go to auto cancel it
		return;
	}

	for(std::list<TimeoutInfo*>::iterator it = timers__Timeouts.begin(); it != timers__Timeouts.end(); it++) {
		TimeoutInfo *other = *it;

		if(other != this) {
			continue;
		}
		
		timers__Timeouts.erase(it);
		break;
	}

	delete this;
}