#ifndef __INCLUDE_USERMESSAGEALREADYSTARTEDEXCEPTIONTYPE_H__
#define __INCLUDE_USERMESSAGEALREADYSTARTEDEXCEPTIONTYPE_H__

#include "Macros.h"
#include "ViperExtension.h"
#include "HL2SDK.h"
#include "BfWriteType.h"

class UserMessageAlreadyStartedExceptionType {
public:
	std::string what() const;
};

#endif