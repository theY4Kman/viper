#include "IntervalInfo.h"
#include "TimersModule.h"
#include "InterfaceContainer.h"

namespace py = boost::python;

IntervalInfo::IntervalInfo(float nextTime, float interval, PyThreadState *threadState, py::object callbackFunction) {
	NextTime = nextTime;
	Interval = interval;
	ThreadState = threadState;
	CallbackFunction = callbackFunction;
}

void IntervalInfo::Execute() {
	PyThreadState *oldThreadState = PyThreadState_Get();

	PyThreadState_Swap(ThreadState);

	try {
		CallbackFunction();
	}
	catch(const py::error_already_set &) {
		PyErr_Print();
	}

	PyThreadState_Swap(oldThreadState);
}

void IntervalInfo::Reset() {
	NextTime = timers__ActualTime + Interval;
}

void IntervalInfo::Cancel() {
	if(!timers__IsIntervalValid(this)) { // just incase people cancel a interval in their callback, then we go to auto cancel it
		return;
	}

	for(std::list<IntervalInfo*>::iterator it = timers__Intervals.begin(); it != timers__Intervals.end(); it++) {
		IntervalInfo *other = *it;

		if(other != this) {
			continue;
		}
		
		timers__Intervals.erase(it);
		break;
	}

	delete this;
}