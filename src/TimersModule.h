#ifndef __INCLUDE_TIMERSMODULE_H__
#define __INCLUDE_TIMERSMODULE_H__

#include "Macros.h"
#include "BoostPythonSM.h"
#include "TimeoutInfo.h"
#include "IntervalInfo.h"

extern "C" __declspec(dllexport) void initTimers();
extern void destroyTimers();
extern void unloadThreadStateTimers(PyThreadState *threadState);

extern void timers__GameFrame(bool simulating);
extern void timers__LevelShutdown();

extern std::list<TimeoutInfo*> timers__Timeouts;
extern std::list<IntervalInfo*> timers__Intervals;

extern bool timers__IsTimeoutValid(TimeoutInfo *info);
extern bool timers__IsIntervalValid(IntervalInfo *info);

extern float timers__ActualTime;

DEFINE_CUSTOM_EXCEPTION_DECL(InvalidTimeoutExceptionType, Timers)
DEFINE_CUSTOM_EXCEPTION_DECL(InvalidIntervalExceptionType, Timers)

#endif