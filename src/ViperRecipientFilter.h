#ifndef __INCLUDE_VIPERRECIPIENTFILTER_H__
#define __INCLUDE_VIPERRECIPIENTFILTER_H__

#include "STL.h"
#include "HL2SDK.h"

class ViperRecipientFilter : public IRecipientFilter {
public:
	ViperRecipientFilter(std::vector<int> clients = std::vector<int>(), bool reliable = false, bool initMessage = false);

	bool IsReliable() const;
	bool IsInitMessage() const;

	int GetRecipientCount() const;
	int GetRecipientIndex(int slot) const;

private:
	std::vector<int> Clients;
	bool Reliable;
	bool InitMessage;
};

#endif