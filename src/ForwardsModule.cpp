#include "BoostPythonSM.h"
#include "ForwardsModule.h"
#include "ForwardType.h"

namespace py = boost::python;

std::map<std::string, ForwardType*> forwards__GlobalForwards;

ForwardType *forwards__GameFrameForward = NULL;
ForwardType *forwards__ClientConnectForward = NULL;
ForwardType *forwards__ClientConnectedForward = NULL;
ForwardType *forwards__ClientPutInServerForward = NULL;
ForwardType *forwards__ClientDisconnectingForward = NULL;
ForwardType *forwards__ClientDisconnectedForward = NULL;
ForwardType *forwards__ClientAuthorizedForward = NULL;
ForwardType *forwards__ServerActivatedForward = NULL;
ForwardType *forwards__ClientPreAdminCheckForward = NULL;
ForwardType *forwards__ClientPostAdminCheckForward = NULL;
ForwardType *forwards__MaxPlayersChangedForward = NULL;
ForwardType *forwards__ClientSettingsChangedForward = NULL;

ForwardsClientListener *forwards_ClientListener = NULL;

// Specific to ClientConnect (Probably could be moved)
bool forwards_ClientConnect_RejectConnection = false;
std::string forwards_ClientConnect_RejectMessage = "";

// Specific to ClientPreAdminCheck (Probably could be moved)
bool forwards_ClientPreAdminCheck_ReturnValue = false;

void forwards__register(std::string globalForwardName, py::object callbackFunction) {
	ForwardType *forward = forwards__GlobalForwards[globalForwardName];
	
	if(NULL == forward) {
		return;
	}

	forward->AddFunction(callbackFunction);
}

ForwardType forwards__create(std::string forwardName, py::object callable, int et) {
	return ForwardType((ForwardCallback)NULL, forwardName);
}

py::object forwards__Forward_Fire(py::tuple argumentsTuple, py::dict keywordsList) {
	py::object self = argumentsTuple[0];

	py::list argList;

	int argCount = py::len(argumentsTuple);

	for(int x = 1; x < argCount; x++) {
		argList.append<py::object>(argumentsTuple[x]);
	}

	ForwardType *forward = py::extract<ForwardType*>(self);

	try {
		forward->Fire(argList);
	}
	catch(const py::error_already_set &) {
		PyErr_Print();
	}

	return BOOST_PY_NONE;
}

void forwards__GameFrame(bool simulating) {
	if(NULL == forwards__GameFrameForward) {
		return;
	}

	py::list args;
	args.append<bool>(simulating);
	
	try {
		forwards__GameFrameForward->Fire(args);
	}
	catch(const py::error_already_set &) {
		PyErr_Print();
	}
}

bool forwards__NoOperationCallback(boost::python::object returnValue) {
	return true;
}

bool forwards__ClientConnectCallback(boost::python::object returnValue) {
	if(PyObject_IsInstance(returnValue.ptr(), (PyObject*)&PyTuple_Type)) {
		if(py::len(returnValue) == 0) {
			forwards_ClientConnect_RejectConnection = false;
			return true;
		}

		if(py::extract<bool>(returnValue[0]) != false) {
			forwards_ClientConnect_RejectConnection = false;
			return true;
		}

		forwards_ClientConnect_RejectConnection = true;
		forwards_ClientConnect_RejectMessage = "";

		if(py::len(returnValue) > 1) {
			forwards_ClientConnect_RejectMessage = py::extract<std::string>(returnValue[1]);
		}

		return false;
	}

	if(py::extract<bool>(returnValue) != false) {
		forwards_ClientConnect_RejectConnection = false;
		return true;
	}

	forwards_ClientConnect_RejectConnection = true;
	forwards_ClientConnect_RejectMessage = "";
	return false;
}

bool forwards__ClientPreAdminCheckCallback(boost::python::object returnValue) {
	if(true == py::extract<bool>(returnValue)) {
		forwards_ClientPreAdminCheck_ReturnValue = true;
		return true;
	}

	forwards_ClientPreAdminCheck_ReturnValue = false;
	return false;
}

BOOST_PYTHON_MODULE(forwards) {
	forwards__GlobalForwards["game_frame"] = forwards__GameFrameForward = new ForwardType(&forwards__NoOperationCallback, "game_frame");
	forwards__GlobalForwards["client_connect"] = forwards__ClientConnectForward = new ForwardType(&forwards__ClientConnectCallback, "client_connect");
	forwards__GlobalForwards["client_connected"] = forwards__ClientConnectedForward = new ForwardType(&forwards__NoOperationCallback, "client_connected");
	forwards__GlobalForwards["client_put_in_server"] = forwards__ClientPutInServerForward = new ForwardType(&forwards__NoOperationCallback, "client_put_in_server");
	forwards__GlobalForwards["client_disconnecting"] = forwards__ClientDisconnectingForward = new ForwardType(&forwards__NoOperationCallback, "client_disconnecting");
	forwards__GlobalForwards["client_disconnected"] = forwards__ClientDisconnectedForward = new ForwardType(&forwards__NoOperationCallback, "client_disconnected");
	forwards__GlobalForwards["client_authorized"] = forwards__ClientAuthorizedForward = new ForwardType(&forwards__NoOperationCallback, "client_authorized");
	forwards__GlobalForwards["server_activated"] = forwards__ServerActivatedForward = new ForwardType(&forwards__NoOperationCallback, "server_activated");
	forwards__GlobalForwards["client_pre_admin_check"] = forwards__ClientPreAdminCheckForward = new ForwardType(&forwards__ClientPreAdminCheckCallback, "client_pre_admin_check");
	forwards__GlobalForwards["client_post_admin_check"] = forwards__ClientPostAdminCheckForward = new ForwardType(&forwards__NoOperationCallback, "client_post_admin_check");
	forwards__GlobalForwards["max_players_changed"] = forwards__MaxPlayersChangedForward = new ForwardType(&forwards__NoOperationCallback, "max_players_changed");
	forwards__GlobalForwards["client_settings_changed"] = forwards__ClientSettingsChangedForward = new ForwardType(&forwards__NoOperationCallback, "client_settings_changed");

	py::class_<ForwardType>("Forward", py::no_init)
		.def("add_function", &ForwardType::AddFunction)
		.def("fire", py::raw_function(&forwards__Forward_Fire, 1))
		.def("get_function_count", &ForwardType::GetFunctionCount)
		.def("remove_function", &ForwardType::RemoveFunction);

	py::def("create", forwards__create);
	py::def("register", forwards__register);

	forwards_ClientListener = new ForwardsClientListener();

	playerhelpers->AddClientListener(forwards_ClientListener);
}

void destroyforwards() {
	for(std::map<std::string, ForwardType*>::iterator it =
		forwards__GlobalForwards.begin(); it !=
		forwards__GlobalForwards.end(); it++) {
		std::pair<std::string, ForwardType*> forwardPair = *it;
		delete forwardPair.second;
	}

	playerhelpers->RemoveClientListener(forwards_ClientListener);

	delete forwards_ClientListener;
}