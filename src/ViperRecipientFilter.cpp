#include "ViperRecipientFilter.h"

ViperRecipientFilter::ViperRecipientFilter(std::vector<int> clients, bool reliable, bool initMessage) {
	Clients = clients;
	Reliable = reliable;
	InitMessage = initMessage;
}

bool ViperRecipientFilter::IsReliable() const {
	return Reliable;
}

bool ViperRecipientFilter::IsInitMessage() const {
	return InitMessage;
}

int ViperRecipientFilter::GetRecipientCount() const {
	return Clients.size();
}

int ViperRecipientFilter::GetRecipientIndex(int slot) const {
	return Clients[slot];
}