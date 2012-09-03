#ifndef __INCLUDE_USERMESSAGESMODULE_H__
#define __INCLUDE_USERMESSAGESMODULE_H__

#include "Macros.h"
#include "BfWriteType.h"
#include "ViperRecipientFilter.h"
#include "UserMessagesUserMessageListener.h"
#include "BoostPythonSM.h"

extern "C" __declspec(dllexport) void initUserMessages();
void destroyUserMessages();
extern void unloadThreadStateUserMessages(PyThreadState *threadState);

DEFINE_CUSTOM_EXCEPTION_DECL(UserMessageAlreadyStartedExceptionType, UserMessages)
DEFINE_CUSTOM_EXCEPTION_DECL(UserMessageIDDoesNotExistExceptionType, UserMessages)
DEFINE_CUSTOM_EXCEPTION_DECL(UserMessageNameDoesNotExistExceptionType, UserMessages)
DEFINE_CUSTOM_EXCEPTION_DECL(UserMessageNotStartedExceptionType, UserMessages)

extern ViperRecipientFilter *usermessages__CurrentRecipientFilter;
extern bool usermessages__AlreadyStarted;

extern std::list<UserMessagesUserMessageListener*> usermessages__Hooks;

#endif