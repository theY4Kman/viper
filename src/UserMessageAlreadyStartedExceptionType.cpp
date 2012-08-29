#include "UserMessageAlreadyStartedExceptionType.h"
#include "Util.h"

std::string UserMessageAlreadyStartedExceptionType::what() const {
	char str[256];
	UTIL_Format(str, sizeof(str), "Another user message has already started");

	return std::string(str);
}