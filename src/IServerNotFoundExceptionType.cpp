#include "IServerNotFoundExceptionType.h"
#include "Util.h"

std::string IServerNotFoundExceptionType::what() const {
	return std::string("Unable to locate IServer");
}