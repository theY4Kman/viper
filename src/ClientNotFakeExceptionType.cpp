#include "ClientNotFakeExceptionType.h"
#include "Util.h"

ClientNotFakeExceptionType::ClientNotFakeExceptionType(int clientIndex) {
	ClientIndex = clientIndex;
}

std::string ClientNotFakeExceptionType::what() const {
	char str[256];
	UTIL_Format(str, sizeof(str), "Client is not fake (index %d)", ClientIndex);

	return std::string(str);
}