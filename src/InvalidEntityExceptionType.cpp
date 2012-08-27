#include "InvalidEntityExceptionType.h"
#include "Util.h"

std::string InvalidEntityExceptionType::what() const {
	return std::string("Invalid entity");
}