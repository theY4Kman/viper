#include "ClientIndexOutOfRangeExceptionType.h"
#include "Util.h"

ClientIndexOutOfRangeExceptionType::ClientIndexOutOfRangeExceptionType(int invalidIndex, int maxClientIndex) {
	InvalidIndex = invalidIndex;
	MaxClientIndex = maxClientIndex;
}

std::string ClientIndexOutOfRangeExceptionType::what() const {
	char str[256];
	UTIL_Format(str, sizeof(str), "Client index %d out of range [1, %d]", InvalidIndex, MaxClientIndex);

	return std::string(str);
}