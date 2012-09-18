#ifndef __INCLUDE_INTERVALINFO_H__
#define __INCLUDE_INTERVALINFO_H__

#include "BoostPythonSM.h"

class IntervalInfo {
public:
	IntervalInfo(float nextTime, float interval, PyThreadState *threadState, boost::python::object callbackFunction);

	void Reset();
	void Execute(); // this does not reset the interval
	void Cancel();

	float NextTime;
	float Interval;
	PyThreadState *ThreadState;
	boost::python::object CallbackFunction;
};

#endif