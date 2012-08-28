#ifndef __INCLUDE_EVENTNOLONGERVALIDEXCEPTIONTYPE_H__
#define __INCLUDE_EVENTNOLONGERVALIDEXCEPTIONTYPE_H__

#include "Macros.h"
#include "ViperExtension.h"
#include "HL2SDK.h"
#include "EventType.h"

class EventNoLongerValidExceptionType {
public:
	EventNoLongerValidExceptionType(EventType *invalidEvent);
	std::string what() const;

private:
	EventType *InvalidEvent;
};

#endif