#include "BoostPythonSM.h"
#include "EventsModule.h"
#include "sdk/smsdk_ext.h"
#include "Macros.h"
#include "ViperExtension.h"
#include "EventType.h"
#include "ModEventsParser.h"
#include "EventHookModes.h"
#include "EventsGameEventListener.h"
#include "EventHookDoesNotExistExceptionType.h"

namespace py = boost::python;

std::map<std::string, std::vector<EventFieldType>> events__ModEvents;
std::vector<EventHook> events__Hooks;
std::map<IGameEvent*, IGameEvent*> events__EventCopies;
std::vector<IGameEvent*> events__CanceledEvents;

#ifdef CreateEvent
#undef CreateEvent
#endif

SH_DECL_HOOK2(IGameEventManager2, FireEvent, SH_NOATTRIB, 0, bool, IGameEvent *,
    bool);

EventType events__create(std::string name, bool force=false) {
	IGameEvent *gameEvent = g_Interfaces.GameEventManagerInstance->CreateEvent(name.c_str(), force);
	
	if(gameEvent == NULL) {
		throw InvalidEventExceptionType(name);
	}
    
	return EventType(name, gameEvent);
}

void events__hook(std::string eventName, py::object pythonCallback, EventHookModes hookMode) {
	if(events__ModEvents.end() == events__ModEvents.find(eventName)) {
		throw InvalidEventExceptionType(eventName);
	}

	PyThreadState *threadState = PyThreadState_Get();

	if(!g_Interfaces.GameEventManagerInstance->FindListener(&events__GameEventListener, eventName.c_str()) &&
		!g_Interfaces.GameEventManagerInstance->AddListener(&events__GameEventListener, eventName.c_str(), true)) {
		throw InvalidEventExceptionType(eventName);
	}

	// lets try to update an existing eventhook
	size_t hookCount = events__Hooks.size();

	for(size_t hookIndex = 0; hookIndex < hookCount; hookIndex++) {
		EventHook eventHook = events__Hooks[hookIndex];

		if(eventHook.EventName != eventName || eventHook.PythonCallback != pythonCallback || eventHook.HookMode != hookMode) {
			continue;
		}

		events__Hooks[hookIndex] = EventHook(eventName, pythonCallback, threadState, hookMode);
		return;
	}
	// otherwise make a new one
	events__Hooks.push_back(EventHook(eventName, pythonCallback, threadState, hookMode));
}

void events__unhook(std::string eventName, py::object pythonCallback, EventHookModes hookMode) {
	if(events__ModEvents.end() == events__ModEvents.find(eventName)) {
		throw InvalidEventExceptionType(eventName);
	}

	size_t hookCount = events__Hooks.size();

	for(size_t hookIndex = 0; hookIndex < hookCount; hookIndex++) {
		EventHook eventHook = events__Hooks[hookIndex];

		if(!eventHook.IsHooked || eventHook.EventName != eventName || eventHook.PythonCallback != pythonCallback || eventHook.HookMode != hookMode) {
			continue;
		}

		eventHook.IsHooked = false;

		events__Hooks[hookIndex] = eventHook;

		return;
	}

	throw EventHookDoesNotExistExceptionType(eventName, hookMode);
}

bool events__OnFireEvent(IGameEvent *gameEvent, bool dontBroadcast) {
	// Make a copy of the current event for any post hooks.
	events__EventCopies[gameEvent] = g_Interfaces.GameEventManagerInstance->DuplicateEvent(gameEvent);

	size_t hookCount = events__Hooks.size();

	for(size_t hookIndex = 0; hookIndex < hookCount; hookIndex++) {
		EventHook eventHook = events__Hooks[hookIndex];

		if(!eventHook.IsHooked || eventHook.HookMode != EventHookMode_Pre) {
			continue;
		}

		std::string eventName = std::string(gameEvent->GetName());

		if(eventHook.EventName != eventName) {
			continue;
		}

		// We need to call the PythonCallback
		PyThreadState *currentThreadState = PyThreadState_Get();

		PyThreadState_Swap(eventHook.ThreadState);

		try {
			eventHook.PythonCallback(EventType(eventName, gameEvent), eventName);
		}
		catch(const py::error_already_set &) {
			PyErr_Print();
		}

		PyThreadState_Swap(currentThreadState);

		// If the event was canceled, we need to break this loop. We need to make sure to preserve the events__CanceledEvents vector so the post hook knows to ignore it
		bool eventCanceled = false;

		for(std::vector<IGameEvent*>::iterator it = events__CanceledEvents.begin(); it != events__CanceledEvents.end(); it++) {
		IGameEvent *canceledEvent = *it;
		
			if(canceledEvent != gameEvent) {
				continue;
			}

			eventCanceled = true;
			break;
		}

		if(true == eventCanceled) {
			break;
		}
	}

	return true;
}

bool events__OnFireEventPost(IGameEvent *gameEvent, bool dontBroadcast) {
	// Retrieve the copy of the event instead.
	IGameEvent *gameEventCopy = events__EventCopies[gameEvent];
	
	for(std::vector<IGameEvent*>::iterator it = events__CanceledEvents.begin(); it != events__CanceledEvents.end(); it++) {
		IGameEvent *canceledEvent = *it;
		
		if(canceledEvent != gameEvent) {
			continue;
		}

		g_Interfaces.GameEventManagerInstance->FreeEvent(gameEventCopy);
		events__CanceledEvents.erase(it);

		return true;
	}

	std::string eventName(gameEventCopy->GetName());

	size_t hookCount = events__Hooks.size();

	for(size_t hookIndex = 0; hookIndex < hookCount; hookIndex++) {
		EventHook eventHook = events__Hooks[hookIndex];

		if(!eventHook.IsHooked || eventHook.HookMode != EventHookMode_Post) {
			continue;
		}

		if(eventHook.EventName != eventName) {
			continue;
		}

		// We need to call the PythonCallback
		PyThreadState *currentThreadState = PyThreadState_Get();

		PyThreadState_Swap(eventHook.ThreadState);
		
		try {
			eventHook.PythonCallback(EventType(eventName, gameEventCopy), eventName);
		}
		catch(const py::error_already_set &) {
			PyErr_Print();
		}

		PyThreadState_Swap(currentThreadState);

		// Okay sometimes these sneaky sneakies will cancel the copy. We need to make sure to break this loop if they did.
		bool copyAlreadyCanceled = false;

		for(std::vector<IGameEvent*>::iterator it = events__CanceledEvents.begin(); it != events__CanceledEvents.end(); it++) {
			IGameEvent *canceledEvent = *it;
		
			if(canceledEvent != gameEventCopy) {
				continue;
			}

			copyAlreadyCanceled = true;
		}

		if(true == copyAlreadyCanceled) {
			break;
		}
	}

	// Yes, we actually have to do this again right here. I don't see a need for a function yet but if we have to copy it again maybe.
	bool copyAlreadyCanceled = false;

	for(std::vector<IGameEvent*>::iterator it = events__CanceledEvents.begin(); it != events__CanceledEvents.end(); it++) {
		IGameEvent *canceledEvent = *it;
		
		if(canceledEvent != gameEventCopy) {
			continue;
		}

		copyAlreadyCanceled = true;
		events__CanceledEvents.erase(it);

		break;
	}

	if(false == copyAlreadyCanceled) {
		g_Interfaces.GameEventManagerInstance->FreeEvent(gameEventCopy);
	}

	events__EventCopies.erase(events__EventCopies.find(gameEvent));

	return true;
}

DEFINE_CUSTOM_EXCEPTION_INIT(InvalidEventExceptionType, events)
DEFINE_CUSTOM_EXCEPTION_INIT(EventNoLongerValidExceptionType, events)
DEFINE_CUSTOM_EXCEPTION_INIT(EventHookDoesNotExistExceptionType, events)


BOOST_PYTHON_MODULE(events) {
	py::enum_<EventHookModes>("EventHookMode")
        .value("Pre", EventHookMode_Pre)
        .value("Post", EventHookMode_Post);

	py::class_<EventType>("Event", py::no_init)
		.def("cancel", &EventType::Cancel)
		.def("fire", &EventType::Fire, (py::arg("dont_broadcast") = false))
		.def("get_fields", &EventType::GetFields)
		.def("is_empty", &EventType::IsEmpty)
		.def("get_name", &EventType::GetName)
		.def("is_reliable", &EventType::IsReliable)
		.def("is_local", &EventType::IsLocal)
		.def("get_bool", &EventType::GetBool, (py::arg("field"), py::arg("default_value") = false))
		.def("get_int", &EventType::GetInt, (py::arg("field"), py::arg("default_value") = 0))
		.def("get_float", &EventType::GetFloat, (py::arg("field"), py::arg("default_value") = 0.0f))
		.def("get_string", &EventType::GetString, (py::arg("field"), py::arg("default_value") = std::string()))
		.def("set_bool", &EventType::SetBool)
		.def("set_int", &EventType::SetInt)
		.def("set_float", &EventType::SetFloat)
		.def("set_string", &EventType::SetString);
	
	py::def("create", events__create);
	py::def("hook", events__hook, (py::arg("event"), py::arg("callback"), py::arg("mode") = EventHookMode_Post));
	py::def("unhook", events__unhook, (py::arg("event"), py::arg("callback"), py::arg("mode") = EventHookMode_Post));

	DEFINE_CUSTOM_EXCEPTION(InvalidEventExceptionType, events,
		PyExc_Exception, "events.InvalidEventException",
		"InvalidEventException")

	DEFINE_CUSTOM_EXCEPTION(EventNoLongerValidExceptionType, events,
		PyExc_Exception, "events.EventNoLongerValidException",
		"EventNoLongerValidException")

	DEFINE_CUSTOM_EXCEPTION(EventHookDoesNotExistExceptionType, events,
		PyExc_Exception, "events.EventHookDoesNotExistException",
		"EventHookDoesNotExistException")

	ModEventsParser modEventsParser;
	events__ModEvents = modEventsParser.Parse();

	// Start hooking the events.
	SH_ADD_HOOK(IGameEventManager2, FireEvent, g_Interfaces.GameEventManagerInstance,
		SH_STATIC(events__OnFireEvent), false);
	SH_ADD_HOOK(IGameEventManager2, FireEvent, g_Interfaces.GameEventManagerInstance,
		SH_STATIC(events__OnFireEventPost), true);
}

void destroyevents() {
	SH_REMOVE_HOOK(IGameEventManager2, FireEvent, g_Interfaces.GameEventManagerInstance,
		SH_STATIC(events__OnFireEvent), false);
	SH_REMOVE_HOOK(IGameEventManager2, FireEvent, g_Interfaces.GameEventManagerInstance,
		SH_STATIC(events__OnFireEventPost), true);

	g_Interfaces.GameEventManagerInstance->RemoveListener(&events__GameEventListener);
}