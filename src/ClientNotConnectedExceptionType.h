#ifndef __INCLUDE_CLIENTNOTCONNECTEDEXCEPTIONTYPE_H__
#define __INCLUDE_CLIENTNOTCONNECTEDEXCEPTIONTYPE_H__

#include "STL.h"

class ClientNotConnectedExceptionType {
public:
	ClientNotConnectedExceptionType(int clientIndex);
	std::string what() const;

private:
	int ClientIndex;
};

#endif