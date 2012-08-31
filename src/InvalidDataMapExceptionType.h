#ifndef __INCLUDE_INVALIDDATAMAPEXCEPTIONTYPE_H__
#define __INCLUDE_INVALIDDATAMAPEXCEPTIONTYPE_H__

#include "Macros.h"
#include "ViperExtension.h"
#include "HL2SDK.h"

class InvalidDataMapExceptionType {
public:
	InvalidDataMapExceptionType(int entityIndex);
	std::string what() const;

private:
	int EntityIndex;
};

#endif