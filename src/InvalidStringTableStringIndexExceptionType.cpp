#include "InvalidStringTableStringIndexExceptionType.h"
#include "Util.h"

InvalidStringTableStringIndexExceptionType::InvalidStringTableStringIndexExceptionType(int stringTableIndex, int invalidStringIndex) {
	StringTableIndex = stringTableIndex;
	InvalidStringIndex = invalidStringIndex;
}

std::string InvalidStringTableStringIndexExceptionType::what() const {
	char str[256];
	UTIL_Format(str, sizeof(str), "Not a valid string table (#%d) string index index: %d", StringTableIndex, InvalidStringIndex);

	return std::string(str);
}