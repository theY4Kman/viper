#ifndef __INCLUDE_ENTITYOFFSETOUTOFRANGEEXCEPTIONTYPE_H__
#define __INCLUDE_ENTITYOFFSETOUTOFRANGEEXCEPTIONTYPE_H__

#include "STL.h"

class EntityOffsetOutOfRangeExceptionType {
public:
	EntityOffsetOutOfRangeExceptionType(int offset);
	std::string what() const;

private:
	int Offset;
};

#endif