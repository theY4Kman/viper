#ifndef __INCLUDE_INVALIDDATAMAPEXCEPTIONTYPE_H__
#define __INCLUDE_INVALIDDATAMAPEXCEPTIONTYPE_H__

#include "STL.h"

class InvalidDataMapExceptionType {
public:
	InvalidDataMapExceptionType(int entityIndex);
	std::string what() const;

private:
	int EntityIndex;
};

#endif