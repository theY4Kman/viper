#ifndef __INCLUDE_ENTITYNOTNETWORKABLEEXCEPTIONTYPE_H__
#define __INCLUDE_ENTITYNOTNETWORKABLEEXCEPTIONTYPE_H__

#include "STL.h"

class EntityNotNetworkableExceptionType {
public:
	EntityNotNetworkableExceptionType(int entityIndex);
	std::string what() const;

private:
	int EntityIndex;
};

#endif