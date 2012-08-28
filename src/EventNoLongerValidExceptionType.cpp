#include "EventNoLongerValidExceptionType.h"
#include "Util.h"

EventNoLongerValidExceptionType::EventNoLongerValidExceptionType(EventType *invalidEvent) {
	InvalidEvent = invalidEvent;
}

std::string EventNoLongerValidExceptionType::what() const {
	char str[256];
	UTIL_Format(str, sizeof(str), "Event is no longer valid: %p", InvalidEvent);

	return std::string(str);
}