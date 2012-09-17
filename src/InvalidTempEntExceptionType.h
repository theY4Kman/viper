#ifndef __INCLUDE_INVALIDTEMPENTEXCEPTIONTYPE_H__
#define __INCLUDE_INVALIDTEMPENTEXCEPTIONTYPE_H__

#include "STL.h"

class InvalidTempEntExceptionType {
public:
	InvalidTempEntExceptionType(std::string invalidTempEntName);
	std::string what() const;

private:
	std::string InvalidTempEntName;
};

#endif