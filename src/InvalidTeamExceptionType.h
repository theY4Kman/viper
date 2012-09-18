#ifndef __INCLUDE_INVALIDTEAMEXCEPTIONTYPE_H__
#define __INCLUDE_INVALIDTEAMEXCEPTIONTYPE_H__

#include "STL.h"

class InvalidTeamExceptionType {
public:
	InvalidTeamExceptionType(int invalidTeamIndex);
	std::string what() const;

private:
	int InvalidTeamIndex;
};

#endif