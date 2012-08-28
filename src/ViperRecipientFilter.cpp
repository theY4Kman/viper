#include "BoostPythonSM.h"
#include "ViperRecipientFilter.h"
#include "Macros.h"
#include "CompatWrappers.h"
#include "ViperExtension.h"

namespace py = boost::python;

ViperRecipientFilter::ViperRecipientFilter(std::vector<int> clients = std::vector<int>(), bool reliable = false, bool initMessage = false) {
	Clients = clients;
	Reliable = reliable;
	InitMessage = initMessage;
}

bool ViperRecipientFilter::IsReliable() {
	return IsReliable;
}

bool ViperRecipientFilter::IsInitMessage() {
	return InitMessage;
}

int ViperRecipientFilter::GetRecipientCount() {
	return Clients.size();
}

int ViperRecipientFilter::GetRecipientIndex(int slot) {
	return Clients[slot];
}