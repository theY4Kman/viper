#ifndef __INCLUDE_INVALIDDATAMAPPROPERTYEXCEPTIONTYPE_H__
#define __INCLUDE_INVALIDDATAMAPPROPERTYEXCEPTIONTYPE_H__

#include "STL.h"

class InvalidDataMapPropertyExceptionType {
public:
	InvalidDataMapPropertyExceptionType(int entityIndex, std::string propertyName);
	std::string what() const;

private:
	int EntityIndex;
	std::string PropertyName;
};

#endif