#ifndef __INCLUDE_ENTITYDATAMAPINFOTYPE_H__
#define __INCLUDE_ENTITYDATAMAPINFOTYPE_H__

#include "BoostPythonSM.h"
#include "Macros.h"
#include "STL.h"
#include "EntityPropertyFieldTypes.h"

class EntityDataMapInfoType {
public:
	EntityDataMapInfoType(EntityPropertyFieldTypes propertyFieldType, unsigned int numBits, int offset);

	EntityPropertyFieldTypes PropertyFieldType;
	unsigned int NumBits;
	int Offset;
};

#endif