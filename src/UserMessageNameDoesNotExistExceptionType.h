#ifndef __INCLUDE_USERMESSAGENAMEDOESNOTEXISTEXCEPTIONTYPE_H__
#define __INCLUDE_USERMESSAGENAMEDOESNOTEXISTEXCEPTIONTYPE_H__

#include "STL.h"

class UserMessageNameDoesNotExistExceptionType {
public:
	UserMessageNameDoesNotExistExceptionType(std::string invalidMessageName);
	std::string what() const;

private:
	std::string InvalidMessageName;
};

#endif