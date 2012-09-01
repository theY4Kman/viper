#include "ConsoleCommandDoesNotExistExceptionType.h"
#include "Util.h"

ConsoleCommandDoesNotExistExceptionType::ConsoleCommandDoesNotExistExceptionType(std::string name) {
	Name = name;
}

std::string ConsoleCommandDoesNotExistExceptionType::what() const {
	char str[256];
	UTIL_Format(str, sizeof(str), "Console command does not exist: %s", Name.c_str());

	return std::string(str);
}