#ifndef __INCLUDE_INVALIDDATAMAPPROPERTYEXCEPTIONTYPE_H__
#define __INCLUDE_INVALIDDATAMAPPROPERTYEXCEPTIONTYPE_H__

#include "Macros.h"
#include "ViperExtension.h"
#include "HL2SDK.h"

class InvalidDataMapPropertyExceptionType {
public:
	InvalidDataMapPropertyExceptionType(int entityIndex, std::string propertyName);
	std::string what() const;

private:
	int EntityIndex;
	std::string PropertyName;
};

#endif