#include "LightStyleOutOfRangeExceptionType.h"
#include "Util.h"
#include "HL2SDK.h"

LightStyleOutOfRangeExceptionType::LightStyleOutOfRangeExceptionType(int outOfRangeLightIndex) {
	OutOfRangeLightIndex = outOfRangeLightIndex;
}

std::string LightStyleOutOfRangeExceptionType::what() const {
	char str[256];
	UTIL_Format(str, sizeof(str), "Light style index out of range (%d): [0, %d]", OutOfRangeLightIndex, MAX_LIGHTSTYLES);

	return std::string(str);
}