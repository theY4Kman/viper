#ifndef __INCLUDE_CLIENTDATANOTAVAILABLEEXCEPTIONTYPE_H__
#define __INCLUDE_CLIENTDATANOTAVAILABLEEXCEPTIONTYPE_H__

#include "STL.h"

class ClientDataNotAvailableExceptionType {
public:
	ClientDataNotAvailableExceptionType(int clientIndex, std::string unavailableData);
	std::string what() const;

private:
	int ClientIndex;
	std::string UnavailableData;
};

#endif