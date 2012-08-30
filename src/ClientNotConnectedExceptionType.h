#ifndef __INCLUDE_CLIENTNOTCONNECTEDEXCEPTIONTYPE_H__
#define __INCLUDE_CLIENTNOTCONNECTEDEXCEPTIONTYPE_H__

#include "ViperExtension.h"

class ClientNotConnectedExceptionType {
public:
	ClientNotConnectedExceptionType(int clientIndex);
	std::string what() const;

private:
	int ClientIndex;
};

#endif