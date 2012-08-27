#ifndef __INCLUDE_INVALIDEDICTEXCEPTIONTYPE_H__
#define __INCLUDE_INVALIDEDICTEXCEPTIONTYPE_H__

#include "ViperExtension.h"

class InvalidEdictExceptionType {
public:
	InvalidEdictExceptionType(edict_t *invalidEdict);
	std::string what() const;

private:
	edict_t *InvalidEdict;
};

#endif