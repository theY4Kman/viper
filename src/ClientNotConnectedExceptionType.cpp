#include "ClientNotConnectedExceptionType.h"
#include "Util.h"

ClientNotConnectedExceptionType::ClientNotConnectedExceptionType(int clientIndex) {
	ClientIndex = clientIndex;
}

std::string ClientNotConnectedExceptionType::what() const {
	char str[256];
	UTIL_Format(str, sizeof(str), "Client not connected (index %d)", ClientIndex);

	return std::string(str);
}