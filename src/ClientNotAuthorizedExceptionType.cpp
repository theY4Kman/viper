#include "ClientNotAuthorizedExceptionType.h"
#include "Util.h"

ClientNotAuthorizedExceptionType::ClientNotAuthorizedExceptionType(int clientIndex) {
	ClientIndex = clientIndex;
}

std::string ClientNotAuthorizedExceptionType::what() const {
	char str[256];
	UTIL_Format(str, sizeof(str), "Client not authorized (index %d)", ClientIndex);

	return std::string(str);
}