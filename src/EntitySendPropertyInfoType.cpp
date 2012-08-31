#include "EntitySendPropertyInfoType.h"
#include "Macros.h"
#include "BoostPythonSM.h"
#include "Util.h"

namespace py = boost::python;

EntitySendPropertyInfoType::EntitySendPropertyInfoType(EntityPropertyFieldTypes propertyFieldType, unsigned int numBits, int localOffset, int absoluteOffset) {
	PropertyFieldType = propertyFieldType;
	NumBits = numBits;
	LocalOffset = localOffset;
	AbsoluteOffset = absoluteOffset;
}