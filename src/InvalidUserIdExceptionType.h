#ifndef __INCLUDE_INVALIDUSERIDEXCEPTIONTYPE_H__
#define __INCLUDE_INVALIDUSERIDEXCEPTIONTYPE_H__

#include "ViperExtension.h"

class InvalidUserIDExceptionType {
public:
	InvalidUserIDExceptionType(int invalidUserId);
	std::string what() const;

private:
	int InvalidUserId;
};

#endif