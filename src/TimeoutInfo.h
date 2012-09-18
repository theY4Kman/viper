#ifndef __INCLUDE_TIMEOUTINFO_H__
#define __INCLUDE_TIMEOUTINFO_H__

#include "BoostPythonSM.h"

class TimeoutInfo {
public:
	TimeoutInfo(float time, float timeout, PyThreadState *threadState, boost::python::object callbackFunction);

	void Cancel();
	void Execute();
	void Reset();

	float Time;
	float Timeout;
	PyThreadState *ThreadState;
	boost::python::object CallbackFunction;
};

#endif