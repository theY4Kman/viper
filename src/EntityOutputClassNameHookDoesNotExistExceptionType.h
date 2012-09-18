#ifndef __INCLUDE_ENTITYOUTPUTCLASSNAMEHOOKDOESNOTEXISTEXCEPTIONTYPE_H__
#define __INCLUDE_ENTITYOUTPUTCLASSNAMEHOOKDOESNOTEXISTEXCEPTIONTYPE_H__

#include "STL.h"

class EntityOutputClassNameHookDoesNotExistExceptionType {
public:
	EntityOutputClassNameHookDoesNotExistExceptionType(std::string className, std::string output);
	std::string what() const;

private:
	std::string ClassName;
	std::string Output;
};

#endif