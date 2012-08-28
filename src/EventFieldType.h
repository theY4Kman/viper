#ifndef __INCLUDE_EVENTFIELDTYPE_H__
#define __INCLUDE_EVENTFIELDTYPE_H__

#include "Macros.h"
#include "ViperExtension.h"
#include "HL2SDK.h"
#include "EdictType.h"
#include "EventFieldTypes.h"

class EventFieldType {
public:
	EventFieldType(std::string name, EventFieldTypes fieldTypes);

public:
	std::string Name;
	EventFieldTypes FieldType;
};

#endif