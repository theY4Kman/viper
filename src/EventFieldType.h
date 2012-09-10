#ifndef __INCLUDE_EVENTFIELDTYPE_H__
#define __INCLUDE_EVENTFIELDTYPE_H__

#include "STL.h"
#include "EventFieldTypes.h"

class EventFieldType {
public:
	EventFieldType(std::string name, EventFieldTypes fieldTypes);

public:
	std::string Name;
	EventFieldTypes FieldType;
};

#endif