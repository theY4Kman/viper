#ifndef __INCLUDE_ENTITYSENDPROPERTYINFOTYPE_H__
#define __INCLUDE_ENTITYSENDPROPERTYINFOTYPE_H__

#include "BoostPythonSM.h"
#include "Macros.h"
#include "STL.h"
#include "EntityPropertyFieldTypes.h"

class EntitySendPropertyInfoType {
public:
	EntitySendPropertyInfoType(EntityPropertyFieldTypes propertyFieldType, unsigned int numBits, int localOffset, int absoluteOffset);

	EntityPropertyFieldTypes PropertyFieldType;
	unsigned int NumBits;
	int LocalOffset;
	int AbsoluteOffset;
};

#endif