#include "InvalidUserIDExceptionType.h"
#include "Util.h"

InvalidUserIDExceptionType::InvalidUserIDExceptionType(int invalidUserId) {
	InvalidUserId = invalidUserId;
}

std::string InvalidUserIDExceptionType::what() const {
	char str[256];
	UTIL_Format(str, sizeof(str), "Invalid user ID: %d", InvalidUserId);

	return std::string(str);
}