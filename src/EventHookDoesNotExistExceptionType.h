#ifndef __INCLUDE_EVENTHOOKDOESNOTEXISTEXCEPTIONTYPE_H__
#define __INCLUDE_EVENTHOOKDOESNOTEXISTEXCEPTIONTYPE_H__

#include "STL.h"
#include "EventHookModes.h"

class EventHookDoesNotExistExceptionType {
public:
	EventHookDoesNotExistExceptionType(std::string eventName, EventHookModes hookMode);
	std::string what() const;

private:
	std::string EventName;
	EventHookModes HookMode;
};

#endif