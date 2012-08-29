#include "UserMessageNotStartedExceptionType.h"
#include "Util.h"

std::string UserMessageNotStartedExceptionType::what() const {
	char str[256];
	UTIL_Format(str, sizeof(str), "%s", "You must start a user message first");

	return std::string(str);
}