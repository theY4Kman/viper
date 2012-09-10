#ifndef __INCLUDE_INVALIDENTITYEXCEPTIONTYPE_H__
#define __INCLUDE_INVALIDENTITYEXCEPTIONTYPE_H__

#include "STL.h"

class InvalidEntityExceptionType {
public:
	InvalidEntityExceptionType(int invalidEntityIndex);
	std::string what() const;

private:
	int InvalidEntityIndex;
};

#endif