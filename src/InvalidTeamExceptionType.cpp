#include "InvalidTeamExceptionType.h"
#include "Util.h"

InvalidTeamExceptionType::InvalidTeamExceptionType(int invalidTeamIndex) {
	InvalidTeamIndex = invalidTeamIndex;
}

std::string InvalidTeamExceptionType::what() const {
	char str[256];
	UTIL_Format(str, sizeof(str), "Not a valid team index: %d", InvalidTeamIndex);

	return std::string(str);
}