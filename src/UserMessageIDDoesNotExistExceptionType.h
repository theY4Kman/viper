#ifndef __INCLUDE_USERMESSAGEIDDOESNOTEXISTEXCEPTIONTYPE_H__
#define __INCLUDE_USERMESSAGEIDDOESNOTEXISTEXCEPTIONTYPE_H__

#include "STL.h"

class UserMessageIDDoesNotExistExceptionType {
public:
	UserMessageIDDoesNotExistExceptionType(int invalidMessageID);
	std::string what() const;

private:
	int InvalidMessageID;
};

#endif