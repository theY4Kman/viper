#include "UserMessagesUserMessageListener.h"
#include "BfReadType.h"
#include "Util.h"

namespace py = boost::python;

UserMessagesUserMessageListener::UserMessagesUserMessageListener(PyThreadState *threadState, int messageID, py::object hookFunction, bool intercept, py::object postHookFunction) {
	ThreadState = threadState;
	MessageID = messageID;
	HookFunction = hookFunction;
	Intercept = intercept;
	PostHookFunction = postHookFunction;
}

void UserMessagesUserMessageListener::OnUserMessage(int messageID, bf_write *bfWrite, IRecipientFilter *recipientFilter) {
	if(Intercept) {
		return;
	}

	py::list recipientList;

	int recipientCount = recipientFilter->GetRecipientCount();

	for(int x = 0; x < recipientCount; x++) {
		recipientList.append<int>(recipientFilter->GetRecipientIndex(x));
	}

	bf_read bfRead;

	bfRead.StartReading(bfWrite->GetBasePointer(), bfWrite->GetNumBytesWritten());

	PyThreadState *originalThreadState = PyThreadState_Get();
	
	PyThreadState_Swap(ThreadState);

	try {
		HookFunction(messageID, BfReadType(&bfRead), recipientList, recipientFilter->IsReliable(), recipientFilter->IsInitMessage());
	}
	catch(const py::error_already_set &) {
		PyErr_Print();
	}

	PyThreadState_Swap(originalThreadState);
}

SourceMod::ResultType UserMessagesUserMessageListener::InterceptUserMessage(int messageID, bf_write *bfWrite, IRecipientFilter *recipientFilter) {
	SourceMod::ResultType result = SourceMod::Pl_Continue;

	if(!Intercept) {
		return result;
	}
	
	py::list recipientList;

	int recipientCount = recipientFilter->GetRecipientCount();

	for(int x = 0; x < recipientCount; x++) {
		recipientList.append<int>(recipientFilter->GetRecipientIndex(x));
	}

	bf_read bfRead;

	bfRead.StartReading(bfWrite->GetBasePointer(), bfWrite->GetNumBytesWritten());

	PyThreadState *originalThreadState = PyThreadState_Get();
	
	PyThreadState_Swap(ThreadState);

	try {
		result = py::extract<SourceMod::ResultType>(HookFunction(messageID, BfReadType(&bfRead), recipientList, recipientFilter->IsReliable(), recipientFilter->IsInitMessage()));
	}
	catch(const py::error_already_set &) {
		PyErr_Print();
	}

	PyThreadState_Swap(originalThreadState);

	return result;
}

void UserMessagesUserMessageListener::OnPostUserMessage(int messageID, bool sent) {
	if(BOOST_PY_NONE == PostHookFunction) {
		return;
	}

	PyThreadState *originalThreadState = PyThreadState_Get();
	
	PyThreadState_Swap(ThreadState);

	try {
		PostHookFunction(messageID, sent);
	}
	catch(const py::error_already_set &) {
		PyErr_Print();
	}

	PyThreadState_Swap(originalThreadState);
}