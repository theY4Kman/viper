#ifndef __INCLUDE_CONSOLECOMMANDDOESNOTEXISTEXCEPTIONTYPE_H__
#define __INCLUDE_CONSOLECOMMANDDOESNOTEXISTEXCEPTIONTYPE_H__

#include "STL.h"

class ConsoleCommandDoesNotExistExceptionType {
public:
	ConsoleCommandDoesNotExistExceptionType(std::string name);
	std::string what() const;

private:
	std::string Name;
};

#endif