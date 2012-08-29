#ifndef __INCLUDE_USERMESSAGENAMEDOESNOTEXISTEXCEPTIONTYPE_H__
#define __INCLUDE_USERMESSAGENAMEDOESNOTEXISTEXCEPTIONTYPE_H__

#include "Macros.h"
#include "ViperExtension.h"
#include "HL2SDK.h"

class UserMessageNameDoesNotExistExceptionType {
public:
	UserMessageNameDoesNotExistExceptionType(std::string invalidMessageName);
	std::string what() const;

private:
	std::string InvalidMessageName;
};

#endif