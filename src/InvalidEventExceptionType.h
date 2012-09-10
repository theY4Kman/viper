#ifndef __INCLUDE_INVALIDEVENTEXCEPTIONTYPE_H__
#define __INCLUDE_INVALIDEVENTEXCEPTIONTYPE_H__

#include "STL.h"

class InvalidEventExceptionType {
public:
	InvalidEventExceptionType(std::string invalidEventName);
	std::string what() const;

private:
	std::string InvalidEventName;
};

#endif