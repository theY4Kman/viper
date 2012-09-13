#include "MapMustBeRunningExceptionType.h"
#include "Util.h"

std::string MapMustBeRunningExceptionType::what() const {
	return std::string("Map must be running");
}