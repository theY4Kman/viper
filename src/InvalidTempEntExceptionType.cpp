#include "InvalidTempEntExceptionType.h"
#include "Util.h"

InvalidTempEntExceptionType::InvalidTempEntExceptionType(std::string invalidTempEntName) {
	InvalidTempEntName = invalidTempEntName;
}

std::string InvalidTempEntExceptionType::what() const {
	char str[256];
	UTIL_Format(str, sizeof(str), "Not a valid temp ent: %s", InvalidTempEntName.c_str());

	return std::string(str);
}