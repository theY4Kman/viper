#ifndef __INCLUDE_CLIENTINDEXOUTOFRANGEEXCEPTIONTYPE_H__
#define __INCLUDE_CLIENTINDEXOUTOFRANGEEXCEPTIONTYPE_H__

#include "STL.h"

class ClientIndexOutOfRangeExceptionType {
public:
	ClientIndexOutOfRangeExceptionType(int invalidIndex, int maxClientIndex);
	std::string what() const;

private:
	int InvalidIndex;
	int MaxClientIndex;
};

#endif