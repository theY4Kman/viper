#ifndef __INCLUDE_SDKTOOLSMODSUPPORTNOTAVAILABLEEXCEPTIONTYPE_H__
#define __INCLUDE_SDKTOOLSMODSUPPORTNOTAVAILABLEEXCEPTIONTYPE_H__

#include "STL.h"

class SDKToolsModSupportNotAvailableExceptionType {
public:
	SDKToolsModSupportNotAvailableExceptionType(std::string unavailableFunctionName);
	std::string what() const;

private:
	std::string UnavailableFunctionName;
};

#endif