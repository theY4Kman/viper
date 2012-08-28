#include "InvalidEventExceptionType.h"
#include "Util.h"

InvalidEventExceptionType::InvalidEventExceptionType(std::string invalidEventName) {
	InvalidEventName = invalidEventName;
}

std::string InvalidEventExceptionType::what() const {
	char str[256];
	UTIL_Format(str, sizeof(str), "Not a valid mod event: %s", InvalidEventName.c_str());

	return std::string(str);
}