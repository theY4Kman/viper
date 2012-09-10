#include "BoostPythonSM.h"
#include "UserMessagesModule.h"
#include "sdk/smsdk_ext.h"
#include "ViperRecipientFilter.h"
#include "BfWriteType.h"
#include "UserMessageAlreadyStartedExceptionType.h"
#include "UserMessageNotStartedExceptionType.h"
#include "UserMessageNameDoesNotExistExceptionType.h"
#include "UserMessageIDDoesNotExistExceptionType.h"

namespace py = boost::python;

ViperRecipientFilter *usermessages__CurrentRecipientFilter = NULL;
bool usermessages__AlreadyStarted = false;
std::list<UserMessagesUserMessageListener*> usermessages__Hooks;

BfWriteType usermessages__start_message(int messageID, boost::python::list clientsList, bool reliable = false, bool initMessage = false) {
	if(usermessages__AlreadyStarted) {
		throw UserMessageAlreadyStartedExceptionType();
	}

	usermessages__AlreadyStarted = true;
	
	std::vector<int> clientsVec;

	int clientsListSize = py::len(clientsList);

	for(int clientIndex = 0; clientIndex < clientsListSize; clientIndex++) {
		clientsVec.push_back(py::extract<int>(clientsList[clientIndex]));
	}

	usermessages__CurrentRecipientFilter = new ViperRecipientFilter(clientsVec, reliable, initMessage);

	bf_write *bfWriteInstance = engine->UserMessageBegin(usermessages__CurrentRecipientFilter, messageID);

	return BfWriteType(bfWriteInstance);
}

int usermessages__get_message_id_by_name(std::string messageName) {
	int messageID = g_SMAPI->FindUserMessage(messageName.c_str());

	if(-1 == messageID) {
		throw UserMessageNameDoesNotExistExceptionType(messageName);
	}

	return messageID;
}

std::string usermessages__get_message_name_by_id(int messageID) {
	const char *messageNameCStr = g_SMAPI->GetUserMessage(messageID);

	if(NULL == messageNameCStr) {
		throw UserMessageIDDoesNotExistExceptionType(messageID);
	}

	return std::string(messageNameCStr);
}

void usermessages__end_message() {
	if(!usermessages__AlreadyStarted) {
		throw UserMessageNotStartedExceptionType();
	}

	engine->MessageEnd();

	delete usermessages__CurrentRecipientFilter;
	usermessages__CurrentRecipientFilter = NULL;

	usermessages__AlreadyStarted = false;
}

void usermessages__hook_message(int messageID, py::object hookFunction, bool intercept = false, py::object postHookFunction = BOOST_PY_NONE) {
	PyThreadState *threadState = PyThreadState_Get();

	UserMessagesUserMessageListener *userMessageListener = new UserMessagesUserMessageListener(threadState, messageID, hookFunction, intercept, postHookFunction);

	if(!usermsgs->HookUserMessage2(messageID, userMessageListener, intercept)) {
		delete userMessageListener;
		throw UserMessageIDDoesNotExistExceptionType(messageID);
	}

	usermessages__Hooks.push_back(userMessageListener);
}

void usermessages__unhook_message(int messageID, py::object hookFunction, bool intercept = false) {
	for(std::list<UserMessagesUserMessageListener*>::iterator it = usermessages__Hooks.begin(); it != usermessages__Hooks.end(); it++) {
		UserMessagesUserMessageListener *userMessageListener = *it;

		if(userMessageListener->MessageID != messageID || userMessageListener->Intercept != intercept || userMessageListener->HookFunction != hookFunction) {
			continue;
		}

		usermsgs->UnhookUserMessage2(messageID, userMessageListener, intercept);
		delete userMessageListener;

		usermessages__Hooks.erase(it);
		return;
	}
}

DEFINE_CUSTOM_EXCEPTION_INIT(UserMessageAlreadyStartedExceptionType, UserMessages)
DEFINE_CUSTOM_EXCEPTION_INIT(UserMessageIDDoesNotExistExceptionType, UserMessages)
DEFINE_CUSTOM_EXCEPTION_INIT(UserMessageNameDoesNotExistExceptionType, UserMessages)
DEFINE_CUSTOM_EXCEPTION_INIT(UserMessageNotStartedExceptionType, UserMessages)

BOOST_PYTHON_MODULE(UserMessages) {
	py::def("StartMessage", usermessages__start_message, (py::arg("message_id"), py::arg("clients"), py::arg("reliable") = false, py::arg("init_message") = false));
	py::def("EndMessage", usermessages__end_message);
	py::def("GetMessageIDByName", usermessages__get_message_id_by_name);
	py::def("GetMessageNameByID", usermessages__get_message_name_by_id);
	py::def("HookMessage", usermessages__hook_message, (py::arg("message_id"), py::arg("hook"), py::arg("intercept") = false, py::arg("post_hook") = BOOST_PY_NONE));
	py::def("UnhookMessage", usermessages__unhook_message, (py::arg("message_id"), py::arg("hook"), py::arg("intercept") = false));

	DEFINE_CUSTOM_EXCEPTION(UserMessageAlreadyStartedExceptionType, UserMessages,
		PyExc_Exception, "UserMessages.UserMessageAlreadyStartedException",
		"UserMessageAlreadyStartedException")

	DEFINE_CUSTOM_EXCEPTION(UserMessageNotStartedExceptionType, UserMessages,
		PyExc_Exception, "UserMessages.UserMessageNotStartedException",
		"UserMessageNotStartedException")

	DEFINE_CUSTOM_EXCEPTION(UserMessageIDDoesNotExistExceptionType, UserMessages,
		PyExc_Exception, "UserMessages.UserMessageIDDoesNotExistException",
		"UserMessageIDDoesNotExistException")

	DEFINE_CUSTOM_EXCEPTION(UserMessageNameDoesNotExistExceptionType, UserMessages,
		PyExc_Exception, "UserMessages.UserMessageNameDoesNotExistException",
		"UserMessageNameDoesNotExistException")
}

void destroyUserMessages() {
	if(usermessages__CurrentRecipientFilter) {
		delete usermessages__CurrentRecipientFilter;
		usermessages__CurrentRecipientFilter = NULL;
	}
}

bool RemoveFirstUserMessageHookByThreadState(PyThreadState *threadState) {
	for(std::list<UserMessagesUserMessageListener*>::iterator it = usermessages__Hooks.begin(); it != usermessages__Hooks.end(); it++) {
		UserMessagesUserMessageListener *userMessageListener = *it;

		if(userMessageListener->ThreadState != threadState) {
			continue;
		}

		usermsgs->UnhookUserMessage2(userMessageListener->MessageID, userMessageListener, userMessageListener->Intercept);
		delete userMessageListener;

		usermessages__Hooks.erase(it);
		return true;
	}

	return false;
}

void RemoveAllUserMessageHooksByThreadState(PyThreadState *threadState) {
	bool searchAgain = true;
	
	while(searchAgain) {
		searchAgain = RemoveFirstUserMessageHookByThreadState(threadState);
	}
}

void unloadThreadStateUserMessages(PyThreadState *threadState) {
	RemoveAllUserMessageHooksByThreadState(threadState);
}