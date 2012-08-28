#ifndef __INCLUDE_VIPERRECIPIENTFILTER_H__
#define __INCLUDE_VIPERRECIPIENTFILTER_H__

#include "Macros.h"
#include "ViperExtension.h"
#include "HL2SDK.h"
#include "EdictType.h"
#include "EventFieldTypes.h"

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