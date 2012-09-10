#ifndef __INCLUDE_INVALIDEDICTEXCEPTIONTYPE_H__
#define __INCLUDE_INVALIDEDICTEXCEPTIONTYPE_H__

#include "STL.h"

class InvalidEdictExceptionType {
public:
	InvalidEdictExceptionType(int invalidEdictIndex);
	std::string what() const;

private:
	int InvalidEdictIndex;
};

#endif