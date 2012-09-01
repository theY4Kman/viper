#include "ConsoleVariableDoesNotExistExceptionType.h"
#include "Util.h"

ConsoleVariableDoesNotExistExceptionType::ConsoleVariableDoesNotExistExceptionType(std::string name) {
	Name = name;
}

std::string ConsoleVariableDoesNotExistExceptionType::what() const {
	char str[256];
	UTIL_Format(str, sizeof(str), "Console variable does not exist: %s", Name.c_str());

	return std::string(str);
}