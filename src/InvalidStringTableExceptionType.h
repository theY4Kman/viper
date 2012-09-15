#ifndef __INCLUDE_INVALIDSTRINGTABLEEXCEPTIONTYPE_H__
#define __INCLUDE_INVALIDSTRINGTABLEEXCEPTIONTYPE_H__

#include "STL.h"

class InvalidStringTableExceptionType {
public:
	InvalidStringTableExceptionType(int invalidStringTableIndex);
	std::string what() const;

private:
	int InvalidStringTableIndex;
};

#endif