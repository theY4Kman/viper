#ifndef __INCLUDE_INVALIDSTRINGTABLESTRINGINDEXEXCEPTIONTYPE_H__
#define __INCLUDE_INVALIDSTRINGTABLESTRINGINDEXEXCEPTIONTYPE_H__

#include "STL.h"

class InvalidStringTableStringIndexExceptionType {
public:
	InvalidStringTableStringIndexExceptionType(int stringTableIndex, int invalidStringIndex);
	std::string what() const;

private:
	int StringTableIndex;
	int InvalidStringIndex;
};

#endif