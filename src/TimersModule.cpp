#include "TimersModule.h"
#include "InterfaceContainer.h"
#include "InvalidTimeoutExceptionType.h"
#include "InvalidIntervalExceptionType.h"
#include "TimeoutType.h"
#include "IntervalType.h"

namespace py = boost::python;

std::list<TimeoutInfo*> timers__Timeouts;
std::list<IntervalInfo*> timers__Intervals;

float timers__ActualTime = 0.0f;
bool timers__HasMapTickedYet = false;
float timers__LastTickedTime = 0.0f;

void timers__LevelShutdown() {
	timers__HasMapTickedYet = false;
}

void timers__GameFrame(bool simulating) {
	if (simulating && timers__HasMapTickedYet) {
		timers__ActualTime += g_Interfaces.GlobalVarsInstance->curtime - timers__LastTickedTime;
	}
	else {
		timers__ActualTime += g_Interfaces.GlobalVarsInstance->interval_per_tick;
	}

	timers__LastTickedTime = g_Interfaces.GlobalVarsInstance->curtime;
	timers__HasMapTickedYet = true;
	
	std::list<TimeoutInfo*> timeoutsCopy = timers__Timeouts;

	for(std::list<TimeoutInfo*>::iterator it = timeoutsCopy.begin(); it != timeoutsCopy.end(); it++) {
		TimeoutInfo *info = *it;

		if(timers__ActualTime < info->Time) {
			continue;
		}

		info->Execute();
	}

	std::list<IntervalInfo*> intervalsCopy = timers__Intervals;

	for(std::list<IntervalInfo*>::iterator it = intervalsCopy.begin(); it != intervalsCopy.end(); it++) {
		IntervalInfo *info = *it;

		if(timers__ActualTime < info->NextTime) {
			continue;
		}

		info->Execute();
		info->Reset();
	}
}

bool timers__IsTimeoutValid(TimeoutInfo *info) {
	if(!info) {
		return false;
	}

	for(std::list<TimeoutInfo*>::iterator it = timers__Timeouts.begin(); it != timers__Timeouts.end(); it++) {
		TimeoutInfo *otherInfo = *it;

		if(info != otherInfo) {
			continue;
		}

		return true;
	}

	return false;
}

bool timers__IsIntervalValid(IntervalInfo *info) {
	if(!info) {
		return false;
	}

	for(std::list<IntervalInfo*>::iterator it = timers__Intervals.begin(); it != timers__Intervals.end(); it++) {
		IntervalInfo *otherInfo = *it;

		if(info != otherInfo) {
			continue;
		}

		return true;
	}

	return false;
}

TimeoutType timers__set_timeout(float delaySeconds, py::object callbackFunction) {
	TimeoutInfo *info = new TimeoutInfo(timers__ActualTime + delaySeconds, delaySeconds, PyThreadState_Get(), callbackFunction);

	timers__Timeouts.push_back(info);

	return TimeoutType(info);
}

IntervalType timers__set_interval(float delaySeconds, py::object callbackFunction) {
	IntervalInfo *info = new IntervalInfo(timers__ActualTime + delaySeconds, delaySeconds, PyThreadState_Get(), callbackFunction);

	timers__Intervals.push_back(info);

	return IntervalType(info);
}

DEFINE_CUSTOM_EXCEPTION_INIT(InvalidTimeoutExceptionType, Timers)
DEFINE_CUSTOM_EXCEPTION_INIT(InvalidIntervalExceptionType, Timers)

BOOST_PYTHON_MODULE(Timers) {
	py::class_<TimeoutType>("Timeout", py::no_init)
		.def("Cancel", &TimeoutType::Cancel)
		.def("Execute", &TimeoutType::Execute)
		.def("Reset", &TimeoutType::Reset);

	py::class_<IntervalType>("Interval", py::no_init)
		.def("Cancel", &IntervalType::Cancel)
		.def("Execute", &IntervalType::Execute, (py::arg("reset") = true))
		.def("Reset", &IntervalType::Reset);

	py::def("SetTimeout", &timers__set_timeout, (py::arg("delay_seconds"), py::arg("callback")));
	py::def("SetInterval", &timers__set_interval, (py::arg("delay_seconds"), py::arg("callback")));

	DEFINE_CUSTOM_EXCEPTION(InvalidTimeoutExceptionType, Timers,
		PyExc_Exception, "Timers.InvalidTimeoutException",
		"InvalidTimeoutException")

	DEFINE_CUSTOM_EXCEPTION(InvalidIntervalExceptionType, Timers,
		PyExc_Exception, "Timers.InvalidIntervalException",
		"InvalidIntervalException")
}

void destroyTimers() {
}

bool RemoveFirstTimeoutByThreadState(PyThreadState *threadState) {
	for(std::list<TimeoutInfo*>::iterator it = timers__Timeouts.begin();
		it != timers__Timeouts.end(); it++) {
		TimeoutInfo *info = *it;

		if(info->ThreadState != threadState) {
			continue;
		}

		timers__Timeouts.erase(it);
		delete info;

		return true;
	}

	return false;
}

void RemoveAllTimeoutsByThreadState(PyThreadState *threadState) {
	bool keepSearching = true;

	while(keepSearching) {
		keepSearching = RemoveFirstTimeoutByThreadState(threadState);
	}
}

bool RemoveFirstIntervalByThreadState(PyThreadState *threadState) {
	for(std::list<IntervalInfo*>::iterator it = timers__Intervals.begin();
		it != timers__Intervals.end(); it++) {
		IntervalInfo *info = *it;

		if(info->ThreadState != threadState) {
			continue;
		}

		timers__Intervals.erase(it);
		delete info;

		return true;
	}

	return false;
}

void RemoveAllIntervalsByThreadState(PyThreadState *threadState) {
	bool keepSearching = true;

	while(keepSearching) {
		keepSearching = RemoveFirstIntervalByThreadState(threadState);
	}
}

void unloadThreadStateTimers(PyThreadState *threadState) {
	RemoveAllTimeoutsByThreadState(threadState);
	RemoveAllIntervalsByThreadState(threadState);
}