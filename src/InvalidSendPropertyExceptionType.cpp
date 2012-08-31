#include "InvalidSendPropertyExceptionType.h"
#include "Util.h"

InvalidSendPropertyExceptionType::InvalidSendPropertyExceptionType(std::string className, std::string propertyName) {
	ClassName = className;
	PropertyName = propertyName;
}

std::string InvalidSendPropertyExceptionType::what() const {
	char str[256];
	UTIL_Format(str, sizeof(str), "Not a valid send property: %s.%s", ClassName.c_str(), PropertyName.c_str());

	return std::string(str);
}