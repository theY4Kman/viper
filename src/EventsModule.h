#ifndef __INCLUDE_EVENTSMODULE_H__
#define __INCLUDE_EVENTSMODULE_H__

#include "Macros.h"
#include "STL.h"
#include "EventFieldType.h"
#include "InvalidEventExceptionType.h"
#include "EventNoLongerValidExceptionType.h"
#include "EventHookDoesNotExistExceptionType.h"
#include "EventHook.h"
#include "BoostPythonSM.h"

extern "C" __declspec(dllexport) void initEvents();
extern void destroyEvents();
extern void unloadThreadStateEvents(PyThreadState *threadState);

extern std::map<std::string, std::vector<EventFieldType>> events__ModEvents;
extern std::list<EventHook> events__Hooks;
extern std::map<IGameEvent*, IGameEvent*> events__EventCopies;
extern std::vector<IGameEvent*> events__CanceledEvents;

DEFINE_CUSTOM_EXCEPTION_DECL(InvalidEventExceptionType, Events)
DEFINE_CUSTOM_EXCEPTION_DECL(EventNoLongerValidExceptionType, Events)
DEFINE_CUSTOM_EXCEPTION_DECL(EventHookDoesNotExistExceptionType, Events)

#endif