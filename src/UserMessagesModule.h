#ifndef __INCLUDE_USERMESSAGESMODULE_H__
#define __INCLUDE_USERMESSAGESMODULE_H__

#include "Macros.h"
#include "BfWriteType.h"
#include "ViperRecipientFilter.h"
#include "UserMessagesUserMessageListener.h"

extern "C" __declspec(dllexport) void initusermessages();
void destroyusermessages();

DEFINE_CUSTOM_EXCEPTION_DECL(UserMessageAlreadyStartedExceptionType, usermessages)
DEFINE_CUSTOM_EXCEPTION_DECL(UserMessageIDDoesNotExistExceptionType, usermessages)
DEFINE_CUSTOM_EXCEPTION_DECL(UserMessageNameDoesNotExistExceptionType, usermessages)
DEFINE_CUSTOM_EXCEPTION_DECL(UserMessageNotStartedExceptionType, usermessages)

extern ViperRecipientFilter *usermessages__CurrentRecipientFilter;
extern bool usermessages__AlreadyStarted;

extern std::vector<UserMessagesUserMessageListener*> usermessages__Hooks;

#endif