#include "InvalidIntervalExceptionType.h"
#include "Util.h"

std::string InvalidIntervalExceptionType::what() const {
	return std::string("Invalid interval");
}