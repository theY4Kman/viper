#ifndef __INCLUDE_INVALIDTEMPENTPROPERTYEXCEPTIONTYPE_H__
#define __INCLUDE_INVALIDTEMPENTPROPERTYEXCEPTIONTYPE_H__

#include "STL.h"

class InvalidTempEntPropertyExceptionType {
public:
	InvalidTempEntPropertyExceptionType(std::string invalidProperty);
	std::string what() const;

private:
	std::string InvalidProperty;
};

#endif