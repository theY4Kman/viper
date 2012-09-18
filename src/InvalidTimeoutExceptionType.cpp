#include "InvalidTimeoutExceptionType.h"
#include "Util.h"

std::string InvalidTimeoutExceptionType::what() const {
	return std::string("Invalid timeout");
}