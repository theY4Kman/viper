#ifndef __INCLUDE_ENTITYNOTNETWORKABLEEXCEPTIONTYPE_H__
#define __INCLUDE_ENTITYNOTNETWORKABLEEXCEPTIONTYPE_H__

#include "Macros.h"
#include "ViperExtension.h"
#include "HL2SDK.h"

class EntityNotNetworkableExceptionType {
public:
	EntityNotNetworkableExceptionType(int entityIndex);
	std::string what() const;

private:
	int EntityIndex;
};

#endif