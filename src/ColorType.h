#ifndef __INCLUDE_COLORTYPE_H__
#define __INCLUDE_COLORTYPE_H__

#include "Macros.h"

class ColorType {
public:
	ColorType(const ColorType &c);
	ColorType(unsigned char r = 255, unsigned char g = 255, unsigned char b = 255, unsigned char a = 255);

	int Compare(ColorType v);
	bool CompareEqual(ColorType v);
	bool CompareNotEqual(ColorType v);
	bool CompareGreaterThan(ColorType v);
	bool CompareLessThan(ColorType v);
	bool CompareGreaterThanOrEqual(ColorType v);
	bool CompareLessThanOrEqual(ColorType v);
	std::string ReprMagic();

	unsigned char Red;
	unsigned char Green;
	unsigned char Blue;
	unsigned char Alpha;
};

#endif