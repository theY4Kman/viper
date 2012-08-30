#ifndef __INCLUDE_INVALIDCLIENTSERIALEXCEPTIONTYPE_H__
#define __INCLUDE_INVALIDCLIENTSERIALEXCEPTIONTYPE_H__

#include "Macros.h"
#include "ViperExtension.h"
#include "HL2SDK.h"

class InvalidClientSerialExceptionType {
public:
	InvalidClientSerialExceptionType(int invalidClientSerial);
	std::string what() const;

private:
	int InvalidClientSerial;
};

#endif