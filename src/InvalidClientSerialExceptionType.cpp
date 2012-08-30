#include "InvalidClientSerialExceptionType.h"
#include "Util.h"

InvalidClientSerialExceptionType::InvalidClientSerialExceptionType(int invalidClientSerial) {
	InvalidClientSerial = invalidClientSerial;
}

std::string InvalidClientSerialExceptionType::what() const {
	char str[256];
	UTIL_Format(str, sizeof(str), "Not a valid client serial: %d", InvalidClientSerial);

	return std::string(str);
}