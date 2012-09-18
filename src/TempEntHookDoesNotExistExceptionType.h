#ifndef __INCLUDE_TEMPENTHOOKDOESNOTEXISTEXCEPTIONTYPE_H__
#define __INCLUDE_TEMPENTHOOKDOESNOTEXISTEXCEPTIONTYPE_H__

#include "STL.h"

class TempEntHookDoesNotExistExceptionType {
public:
	TempEntHookDoesNotExistExceptionType(std::string effectName);
	std::string what() const;

private:
	std::string EffectName;
};

#endif