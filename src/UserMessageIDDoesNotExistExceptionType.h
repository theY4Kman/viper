#ifndef __INCLUDE_USERMESSAGEIDDOESNOTEXISTEXCEPTIONTYPE_H__
#define __INCLUDE_USERMESSAGEIDDOESNOTEXISTEXCEPTIONTYPE_H__

#include "Macros.h"
#include "ViperExtension.h"
#include "HL2SDK.h"

class UserMessageIDDoesNotExistExceptionType {
public:
	UserMessageIDDoesNotExistExceptionType(int invalidMessageID);
	std::string what() const;

private:
	int InvalidMessageID;
};

#endif