#include "InvalidUserIdExceptionType.h"
#include "Util.h"

InvalidUserIdExceptionType::InvalidUserIdExceptionType(int invalidUserId) {
	InvalidUserId = invalidUserId;
}

std::string InvalidUserIdExceptionType::what() const {
	char str[256];
	UTIL_Format(str, sizeof(str), "Invalid user ID: %d", InvalidUserId);

	return std::string(str);
}