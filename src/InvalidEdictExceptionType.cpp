#include "InvalidEdictExceptionType.h"
#include "Util.h"

InvalidEdictExceptionType::InvalidEdictExceptionType(int invalidEdictIndex) {
	InvalidEdictIndex = invalidEdictIndex;
}

std::string InvalidEdictExceptionType::what() const {
	char str[256];
	UTIL_Format(str, sizeof(str), "Invalid edict: #%d", InvalidEdictIndex);

	return std::string(str);
}