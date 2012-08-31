#include "BoostPythonSM.h"
#include "UserMessagesModule.h"
#include "sdk/smsdk_ext.h"
#include "Macros.h"
#include "ViperExtension.h"
#include "ViperRecipientFilter.h"
#include "BfWriteType.h"
#include "UserMessageAlreadyStartedExceptionType.h"
#include "UserMessageNotStartedExceptionType.h"
#include "UserMessageNameDoesNotExistExceptionType.h"
#include "UserMessageIDDoesNotExistExceptionType.h"

namespace py = boost::python;

ViperRecipientFilter *usermessages__CurrentRecipientFilter = NULL;
bool usermessages__AlreadyStarted = false;
std::vector<UserMessagesUserMessageListener*> usermessages__Hooks;

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
	for(std::vector<UserMessagesUserMessageListener*>::iterator it = usermessages__Hooks.begin(); it != usermessages__Hooks.end(); it++) {
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

DEFINE_CUSTOM_EXCEPTION_INIT(UserMessageAlreadyStartedExceptionType, usermessages)
DEFINE_CUSTOM_EXCEPTION_INIT(UserMessageIDDoesNotExistExceptionType, usermessages)
DEFINE_CUSTOM_EXCEPTION_INIT(UserMessageNameDoesNotExistExceptionType, usermessages)
DEFINE_CUSTOM_EXCEPTION_INIT(UserMessageNotStartedExceptionType, usermessages)

BOOST_PYTHON_MODULE(usermessages) {
	py::def("start_message", usermessages__start_message, (py::arg("message_id"), py::arg("clients"), py::arg("reliable") = false, py::arg("init_message") = false));
	py::def("end_message", usermessages__end_message);
	py::def("get_message_id_by_name", usermessages__get_message_id_by_name);
	py::def("get_message_name_by_id", usermessages__get_message_name_by_id);
	py::def("hook_message", usermessages__hook_message, (py::arg("message_id"), py::arg("hook"), py::arg("intercept") = false, py::arg("post_hook") = BOOST_PY_NONE));
	py::def("unhook_message", usermessages__unhook_message, (py::arg("message_id"), py::arg("hook"), py::arg("intercept") = false));

	DEFINE_CUSTOM_EXCEPTION(UserMessageAlreadyStartedExceptionType, usermessages,
		PyExc_Exception, "usermessages.UserMessageAlreadyStartedException",
		"UserMessageAlreadyStartedException")

	DEFINE_CUSTOM_EXCEPTION(UserMessageNotStartedExceptionType, usermessages,
		PyExc_Exception, "usermessages.UserMessageNotStartedException",
		"UserMessageNotStartedException")

	DEFINE_CUSTOM_EXCEPTION(UserMessageIDDoesNotExistExceptionType, usermessages,
		PyExc_Exception, "usermessages.UserMessageIDDoesNotExistException",
		"UserMessageIDDoesNotExistException")

	DEFINE_CUSTOM_EXCEPTION(UserMessageNameDoesNotExistExceptionType, usermessages,
		PyExc_Exception, "usermessages.UserMessageNameDoesNotExistException",
		"UserMessageNameDoesNotExistException")
}

void destroyusermessages() {
	if(usermessages__CurrentRecipientFilter) {
		delete usermessages__CurrentRecipientFilter;
		usermessages__CurrentRecipientFilter = NULL;
	}
}