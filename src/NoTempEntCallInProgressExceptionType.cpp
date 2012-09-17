#include "NoTempEntCallInProgressExceptionType.h"
#include "Util.h"

std::string NoTempEntCallInProgressExceptionType::what() const {
	return std::string("No temp ent call in progress");
}