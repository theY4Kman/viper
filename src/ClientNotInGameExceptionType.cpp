#include "ClientNotInGameExceptionType.h"
#include "Util.h"

ClientNotInGameExceptionType::ClientNotInGameExceptionType(int clientIndex) {
	ClientIndex = clientIndex;
}

std::string ClientNotInGameExceptionType::what() const {
	char str[256];
	UTIL_Format(str, sizeof(str), "Client not in game (index %d)", ClientIndex);

	return std::string(str);
}