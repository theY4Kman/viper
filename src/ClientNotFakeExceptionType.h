#ifndef __INCLUDE_CLIENTNOTFAKEEXCEPTIONTYPE_H__
#define __INCLUDE_CLIENTNOTFAKEEXCEPTIONTYPE_H__

#include "STL.h"

class ClientNotFakeExceptionType {
public:
	ClientNotFakeExceptionType(int clientIndex);
	std::string what() const;

private:
	int ClientIndex;
};

#endif