#include "EntityOffsetOutOfRangeExceptionType.h"
#include "Util.h"

EntityOffsetOutOfRangeExceptionType::EntityOffsetOutOfRangeExceptionType(int offset) {
	Offset = offset;
}

std::string EntityOffsetOutOfRangeExceptionType::what() const {
	char str[256];
	UTIL_Format(str, sizeof(str), "Entity offset #%d out of range: [1, 32768]", Offset);

	return std::string(str);
}