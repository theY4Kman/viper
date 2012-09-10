#ifndef __INCLUDE_EVENTNOLONGERVALIDEXCEPTIONTYPE_H__
#define __INCLUDE_EVENTNOLONGERVALIDEXCEPTIONTYPE_H__

#include "STL.h"
#include "EventType.h"

class EventNoLongerValidExceptionType {
public:
	EventNoLongerValidExceptionType(EventType *invalidEvent);
	std::string what() const;

private:
	EventType *InvalidEvent;
};

#endif