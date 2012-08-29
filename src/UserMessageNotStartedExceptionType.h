#ifndef __INCLUDE_USERMESSAGENOTSTARTEDEXCEPTIONTYPE_H__
#define __INCLUDE_USERMESSAGENOTSTARTEDEXCEPTIONTYPE_H__

#include "Macros.h"
#include "ViperExtension.h"
#include "HL2SDK.h"
#include "BfWriteType.h"

class UserMessageNotStartedExceptionType {
public:
	std::string what() const;
};

#endif