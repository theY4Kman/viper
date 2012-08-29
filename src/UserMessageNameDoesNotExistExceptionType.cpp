#include "UserMessageNameDoesNotExistExceptionType.h"
#include "Util.h"

UserMessageNameDoesNotExistExceptionType::UserMessageNameDoesNotExistExceptionType(std::string invalidMessageName) {
	InvalidMessageName = invalidMessageName;
}

std::string UserMessageNameDoesNotExistExceptionType::what() const {
	char str[256];
	UTIL_Format(str, sizeof(str), "User message \"%s\" does not exist", InvalidMessageName.c_str());

	return std::string(str);
}