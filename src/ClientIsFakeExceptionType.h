#ifndef __INCLUDE_CLIENTISFAKEEXCEPTIONTYPE_H__
#define __INCLUDE_CLIENTISFAKEEXCEPTIONTYPE_H__

#include "STL.h"

class ClientIsFakeExceptionType {
public:
	ClientIsFakeExceptionType(int clientIndex);
	std::string what() const;

private:
	int ClientIndex;
};

#endif