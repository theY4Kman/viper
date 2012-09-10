#ifndef __INCLUDE_INVALIDCLIENTSERIALEXCEPTIONTYPE_H__
#define __INCLUDE_INVALIDCLIENTSERIALEXCEPTIONTYPE_H__

#include "STL.h"

class InvalidClientSerialExceptionType {
public:
	InvalidClientSerialExceptionType(int invalidClientSerial);
	std::string what() const;

private:
	int InvalidClientSerial;
};

#endif