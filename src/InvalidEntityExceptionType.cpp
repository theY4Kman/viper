#include "InvalidEntityExceptionType.h"
#include "Util.h"

InvalidEntityExceptionType::InvalidEntityExceptionType(int invalidEntityIndex) {
	InvalidEntityIndex = invalidEntityIndex;
}

std::string InvalidEntityExceptionType::what() const {
	char str[256];
	UTIL_Format(str, sizeof(str), "Invalid entity: #%d", InvalidEntityIndex);

	return std::string(str);
}