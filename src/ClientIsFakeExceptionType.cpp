#include "ClientIsFakeExceptionType.h"
#include "Util.h"

ClientIsFakeExceptionType::ClientIsFakeExceptionType(int clientIndex) {
	ClientIndex = clientIndex;
}

std::string ClientIsFakeExceptionType::what() const {
	char str[256];
	UTIL_Format(str, sizeof(str), "Client is fake (index %d)", ClientIndex);

	return std::string(str);
}