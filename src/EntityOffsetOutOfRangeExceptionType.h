#ifndef __INCLUDE_ENTITYOFFSETOUTOFRANGEEXCEPTIONTYPE_H__
#define __INCLUDE_ENTITYOFFSETOUTOFRANGEEXCEPTIONTYPE_H__

#include "Macros.h"
#include "ViperExtension.h"
#include "HL2SDK.h"

class EntityOffsetOutOfRangeExceptionType {
public:
	EntityOffsetOutOfRangeExceptionType(int offset);
	std::string what() const;

private:
	int Offset;
};

#endif