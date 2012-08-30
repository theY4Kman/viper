#ifndef __INCLUDE_CLIENTNOTINGAMEEXCEPTIONTYPE_H__
#define __INCLUDE_CLIENTNOTINGAMEEXCEPTIONTYPE_H__

#include "ViperExtension.h"

class ClientNotInGameExceptionType {
public:
	ClientNotInGameExceptionType(int clientIndex);
	std::string what() const;

private:
	int ClientIndex;
};

#endif