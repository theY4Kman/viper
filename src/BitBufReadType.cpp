#include "BitBufReadType.h"
#include "Macros.h"
#include "BoostPythonSM.h"
#include "Util.h"
#include "NullReferenceExceptionType.h"

namespace py = boost::python;

BitBufReadType::BitBufReadType(bf_read *bfRead) {
	if(bfRead == NULL) {
		throw NullReferenceExceptionType();
	}

	BfRead = bfRead;
}

float BitBufReadType::ReadAngle(int numBits) {
	return BfRead->ReadBitAngle(numBits);
}

VectorType BitBufReadType::ReadAngles() {
	QAngle qAngle;
	BfRead->ReadBitAngles(qAngle);

	return VectorType(qAngle.x,	qAngle.y, qAngle.z);
}

std::string BitBufReadType::StrMagic() {
	char str[256];
	UTIL_Format(str, sizeof(str), "<BitBufRead object at: %p>", this);

	return std::string(str);
}