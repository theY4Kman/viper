#ifndef __INCLUDE_EVENTHOOK_H__
#define __INCLUDE_EVENTHOOK_H__

#include "BoostPythonSM.h"
#include "Macros.h"
#include "STL.h"
#include "EventHookModes.h"

class EventHook {
public:
	EventHook(std::string eventName, boost::python::object pythonCallback, PyThreadState *threadState, EventHookModes hookMode);

	std::string EventName;
	boost::python::object PythonCallback;
	PyThreadState *ThreadState;
	EventHookModes HookMode;
};

#endif