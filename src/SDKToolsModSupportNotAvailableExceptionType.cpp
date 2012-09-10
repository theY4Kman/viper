#include "SDKToolsModSupportNotAvailableExceptionType.h"
#include "Util.h"

SDKToolsModSupportNotAvailableExceptionType::SDKToolsModSupportNotAvailableExceptionType(std::string unavailableFunctionName) {
	UnavailableFunctionName = unavailableFunctionName;
}

std::string SDKToolsModSupportNotAvailableExceptionType::what() const {
	char str[256];
	UTIL_Format(str, sizeof(str), "SDKTools function not available for current mod: %s)", UnavailableFunctionName.c_str());

	return std::string(str);
}