#include "UserMessageIDDoesNotExistExceptionType.h"
#include "Util.h"

UserMessageIDDoesNotExistExceptionType::UserMessageIDDoesNotExistExceptionType(int invalidMessageID) {
	InvalidMessageID = invalidMessageID;
}

std::string UserMessageIDDoesNotExistExceptionType::what() const {
	char str[256];
	UTIL_Format(str, sizeof(str), "User message #%d does not exist", InvalidMessageID);

	return std::string(str);
}