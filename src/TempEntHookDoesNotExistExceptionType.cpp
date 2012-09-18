#include "TempEntHookDoesNotExistExceptionType.h"
#include "Util.h"

TempEntHookDoesNotExistExceptionType::TempEntHookDoesNotExistExceptionType(std::string effectName) {
	EffectName = effectName;
}

std::string TempEntHookDoesNotExistExceptionType::what() const {
	char str[256];
	UTIL_Format(str, sizeof(str), "Event hook does not exist (%s)", EffectName.c_str());

	return std::string(str);
}