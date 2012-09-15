#include "InvalidStringTableExceptionType.h"
#include "Util.h"

InvalidStringTableExceptionType::InvalidStringTableExceptionType(int invalidStringTableIndex) {
	InvalidStringTableIndex = invalidStringTableIndex;
}

std::string InvalidStringTableExceptionType::what() const {
	char str[256];
	UTIL_Format(str, sizeof(str), "Not a valid string table index: %d", InvalidStringTableIndex);

	return std::string(str);
}