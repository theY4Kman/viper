#include "ClientDataNotAvailableExceptionType.h"
#include "Util.h"

ClientDataNotAvailableExceptionType::ClientDataNotAvailableExceptionType(int clientIndex, std::string unavailableData) {
	ClientIndex = clientIndex;
	UnavailableData = unavailableData;
}

std::string ClientDataNotAvailableExceptionType::what() const {
	char str[256];
	UTIL_Format(str, sizeof(str), "Client data (index %d) not available (%s)", ClientIndex, UnavailableData.c_str());

	return std::string(str);
}