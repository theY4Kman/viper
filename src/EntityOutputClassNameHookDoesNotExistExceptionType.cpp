#include "EntityOutputClassNameHookDoesNotExistExceptionType.h"
#include "Util.h"

EntityOutputClassNameHookDoesNotExistExceptionType::EntityOutputClassNameHookDoesNotExistExceptionType(std::string className, std::string output) {
	ClassName = className;
	Output = output;
}

std::string EntityOutputClassNameHookDoesNotExistExceptionType::what() const {
	char str[256];
	UTIL_Format(str, sizeof(str), "Entity output classname hook does not exist (%s %s)", ClassName.c_str(), Output.c_str());

	return std::string(str);
}