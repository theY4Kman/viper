#ifndef __INCLUDE_LIGHTSTYLEOUTOFRANGEEXCEPTIONTYPE_H__
#define __INCLUDE_LIGHTSTYLEOUTOFRANGEEXCEPTIONTYPE_H__

#include "STL.h"

class LightStyleOutOfRangeExceptionType {
public:
	LightStyleOutOfRangeExceptionType(int outOfRangeLightIndex);
	std::string what() const;

private:
	int OutOfRangeLightIndex;
};

#endif