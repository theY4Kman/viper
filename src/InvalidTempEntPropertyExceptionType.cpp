#include "InvalidTempEntPropertyExceptionType.h"
#include "Util.h"

InvalidTempEntPropertyExceptionType::InvalidTempEntPropertyExceptionType(std::string invalidProperty) {
	InvalidProperty = invalidProperty;
}

std::string InvalidTempEntPropertyExceptionType::what() const {
	char str[256];
	UTIL_Format(str, sizeof(str), "Invalid temp ent property: %s", InvalidProperty.c_str());

	return std::string(str);
}