#ifndef __INCLUDE_CONSOLEVARIABLEDOESNOTEXISTEXCEPTIONTYPE_H__
#define __INCLUDE_CONSOLEVARIABLEDOESNOTEXISTEXCEPTIONTYPE_H__

#include "STL.h"

class ConsoleVariableDoesNotExistExceptionType {
public:
	ConsoleVariableDoesNotExistExceptionType(std::string name);
	std::string what() const;

private:
	std::string Name;
};

#endif