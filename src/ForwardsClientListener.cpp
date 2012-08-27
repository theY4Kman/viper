#include "ForwardsClientListener.h"
#include "BoostPythonSM.h"
#include "ForwardsModule.h"
#include "Util.h"
#include "ClientType.h"

namespace py = boost::python;

bool ForwardsClientListener::InterceptClientConnect(int clientIndex, char *error, size_t maxlength) {
	ClientType client(clientIndex);

	py::list args;
	args.append<ClientType>(client);

	try {
		forwards__ClientConnectForward->Fire(args);
	}
	catch(const py::error_already_set &) {
		PyErr_Print();
	}

	if(false == forwards_ClientConnect_RejectConnection) {
		return true;
	}

	UTIL_Format(error, maxlength, "%s", forwards_ClientConnect_RejectMessage.c_str());
	RETURN_META_VALUE(MRES_SUPERCEDE, false); // TODO:: WE SHOULD NOT HAVE TO DO THIS!!!
}

void ForwardsClientListener::OnClientConnected(int clientIndex) {
	ClientType client(clientIndex);

	py::list args;
	args.append<ClientType>(client);

	try {
		forwards__ClientConnectedForward->Fire(args);
	}
	catch(const py::error_already_set &) {
		PyErr_Print();
	}
}

void ForwardsClientListener::OnClientPutInServer(int clientIndex) {
	ClientType client(clientIndex);

	py::list args;
	args.append<ClientType>(client);

	try {
		forwards__ClientPutInServerForward->Fire(args);
	}
	catch(const py::error_already_set &) {
		PyErr_Print();
	}
}

void ForwardsClientListener::OnClientDisconnecting(int clientIndex) {
	ClientType client(clientIndex);

	py::list args;
	args.append<ClientType>(client);

	try {
		forwards__ClientDisconnectingForward->Fire(args);
	}
	catch(const py::error_already_set &) {
		PyErr_Print();
	}
}

void ForwardsClientListener::OnClientDisconnected(int clientIndex) {
	ClientType client(clientIndex);

	py::list args;
	args.append<ClientType>(client);

	try {
		forwards__ClientDisconnectedForward->Fire(args);
	}
	catch(const py::error_already_set &) {
		PyErr_Print();
	}
}

void ForwardsClientListener::OnClientAuthorized(int clientIndex, const char *authstring) {
	ClientType client(clientIndex);

	py::list args;
	args.append<ClientType>(client);
	args.append<std::string>(std::string(authstring));

	try {
		forwards__ClientAuthorizedForward->Fire(args);
	}
	catch(const py::error_already_set &) {
		PyErr_Print();
	}
}

void ForwardsClientListener::OnServerActivated(int max_clients) {
	py::list args;
	args.append<int>(max_clients);
	
	try {
		forwards__ServerActivatedForward->Fire(args);
	}
	catch(const py::error_already_set &) {
		PyErr_Print();
	}
}

bool ForwardsClientListener::OnClientPreAdminCheck(int clientIndex) {
	ClientType client(clientIndex);

	py::list args;
	args.append<ClientType>(client);
	
	try {
		forwards__ClientPreAdminCheckForward->Fire(args);
	}
	catch(const py::error_already_set &) {
		PyErr_Print();
	}

	return forwards_ClientPreAdminCheck_ReturnValue;
}

void ForwardsClientListener::OnClientPostAdminCheck(int clientIndex) {
	ClientType client(clientIndex);

	py::list args;
	args.append<ClientType>(client);
	
	try {
		forwards__ClientPostAdminCheckForward->Fire(args);
	}
	catch(const py::error_already_set &) {
		PyErr_Print();
	}

}

void ForwardsClientListener::OnMaxPlayersChanged(int newvalue) {
	py::list args;
	args.append<int>(newvalue);
	
	try {
		forwards__MaxPlayersChangedForward->Fire(args);
	}
	catch(const py::error_already_set &) {
		PyErr_Print();
	}
}

void ForwardsClientListener::OnClientSettingsChanged(int clientIndex) {
	ClientType client(clientIndex);

	py::list args;
	args.append<ClientType>(client);
	
	try {
		forwards__ClientSettingsChangedForward->Fire(args);
	}
	catch(const py::error_already_set &) {
		PyErr_Print();
	}
}