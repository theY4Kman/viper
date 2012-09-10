#ifndef __INCLUDE_MODEVENTSPARSER_H__
#define __INCLUDE_MODEVENTSPARSER_H__

#include "STL.h"
#include "EventFieldType.h"

class ModEventsParser {
public:
	std::map<std::string, std::vector<EventFieldType>> Parse();
};

#endif

// modevents.res