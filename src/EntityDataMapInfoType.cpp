#include "EntityDataMapInfoType.h"
#include "Macros.h"
#include "BoostPythonSM.h"
#include "Util.h"

namespace py = boost::python;

EntityDataMapInfoType::EntityDataMapInfoType(EntityPropertyFieldTypes propertyFieldType, unsigned int numBits, int offset) {
	PropertyFieldType = propertyFieldType;
	NumBits = numBits;
	Offset = offset;
}