#include "EventHookDoesNotExistExceptionType.h"
#include "Util.h"

EventHookDoesNotExistExceptionType::EventHookDoesNotExistExceptionType(std::string eventName, EventHookModes hookMode) {
	EventName = eventName;
	HookMode = hookMode;
}

std::string EventHookDoesNotExistExceptionType::what() const {
	char str[256];
	UTIL_Format(str, sizeof(str), "Event hook (%s hook mode #%d) does not exist", EventName, HookMode);

	return std::string(str);
}