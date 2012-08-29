#include "BfReadType.h"
#include "Macros.h"
#include "BoostPythonSM.h"
#include "Util.h"
#include "NullReferenceExceptionType.h"

namespace py = boost::python;

BfReadType::BfReadType(bf_read *bfReadInstance) {
	if(bfReadInstance == NULL) {
		throw NullReferenceExceptionType();
	}

	BfReadInstance = bfReadInstance;
}

float BfReadType::ReadAngle(int numBits) {
	return BfReadInstance->ReadBitAngle(numBits);
}

VectorType BfReadType::ReadAngles() {
	QAngle qAngle;
	BfReadInstance->ReadBitAngles(qAngle);

	return VectorType(qAngle.x,	qAngle.y, qAngle.z);
}

std::string BfReadType::StrMagic() {
	char str[256];
	UTIL_Format(str, sizeof(str), "<BitBufRead object at: %p>", this);

	return std::string(str);
}