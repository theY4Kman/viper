#ifndef __INCLUDE_INVALIDSENDPROPERTYEXCEPTIONTYPE_H__
#define __INCLUDE_INVALIDSENDPROPERTYEXCEPTIONTYPE_H__

#include "Macros.h"
#include "ViperExtension.h"
#include "HL2SDK.h"

class InvalidSendPropertyExceptionType {
public:
	InvalidSendPropertyExceptionType(std::string className, std::string propertyName);
	std::string what() const;

private:
	std::string ClassName;
	std::string PropertyName;
};

#endif