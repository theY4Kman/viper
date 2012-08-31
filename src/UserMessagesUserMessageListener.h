#ifndef __INCLUDE_USERMESSAGESUSERMESSAGELISTENER_H__
#define __INCLUDE_USERMESSAGESUSERMESSAGELISTENER_H__

#include "Macros.h"
#include "ViperExtension.h"
#include "HL2SDK.h"
#include "BfWriteType.h"

class UserMessagesUserMessageListener : public SourceMod::IUserMessageListener {
public:
	UserMessagesUserMessageListener(PyThreadState *threadState, int messageID, boost::python::object hookFunction,
		bool intercept = false, boost::python::object postHookFunction = boost::python::object());
	
	void OnUserMessage(int messageID, bf_write *bfWrite, IRecipientFilter *recipientFilter);
	SourceMod::ResultType InterceptUserMessage(int messageID, bf_write *bfWrite, IRecipientFilter *recipientFilter);
	void OnPostUserMessage(int messageID, bool sent);

	int MessageID;
	boost::python::object HookFunction;
	bool Intercept;
	boost::python::object PostHookFunction;
	PyThreadState *ThreadState;
};

#endif