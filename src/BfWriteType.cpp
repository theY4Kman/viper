#include "BfWriteType.h"
#include "Macros.h"
#include "BoostPythonSM.h"
#include "BitBufVectorException.h"
#include "Util.h"
#include "NullReferenceExceptionType.h"

namespace py = boost::python;

BfWriteType::BfWriteType(bf_write *bfWriteInstance) {
	if(NULL == bfWriteInstance) {
		throw NullReferenceExceptionType();
	}

	BfWriteInstance = bfWriteInstance;
}

void BfWriteType::WriteAngle(float num, int numBits) {
	BfWriteInstance->WriteBitAngle(num, numBits);
}

void BfWriteType::WriteAngles(py::list anglesList) {
	if(py::len(anglesList) != 3) {
		throw BitBufVectorException(anglesList);
	}

	BfWriteInstance->WriteBitAngles(QAngle(py::extract<float>(anglesList[0]), py::extract<float>(anglesList[1]), py::extract<float>(anglesList[2])));
}

void BfWriteType::WriteBool(bool boolValue) {
	BfWriteInstance->WriteOneBit(boolValue ? 1 : 0);
}

void BfWriteType::WriteByte(int byte) {			
	BfWriteInstance->WriteByte(byte);
}

void BfWriteType::WriteChar(char charValue) {			
	BfWriteInstance->WriteChar(charValue);
}

void BfWriteType::WriteShort(int shortValue) {
	BfWriteInstance->WriteShort(shortValue);
}

void BfWriteType::WriteString(std::string stringValue) {
	BfWriteInstance->WriteString(stringValue.c_str());
}
			
void BfWriteType::WriteVecCoord(py::list vecCoord) {
	if(py::len(vecCoord) != 3) {
		throw BitBufVectorException(vecCoord);
	}

	BfWriteInstance->WriteBitVec3Coord(Vector(py::extract<float>(vecCoord[0]), py::extract<float>(vecCoord[1]), py::extract<float>(vecCoord[2])));
}
			
void BfWriteType::WriteVecNormal(boost::python::list vecNormal) {
	if(py::len(vecNormal) != 3) {
		throw BitBufVectorException(vecNormal);
	}

	BfWriteInstance->WriteBitVec3Normal(Vector(py::extract<float>(vecNormal[0]), py::extract<float>(vecNormal[1]), py::extract<float>(vecNormal[2])));
}

void BfWriteType::WriteWord(int word) {
	BfWriteInstance->WriteWord(word);
}

void BfWriteType::WriteEntity(int entityIndex) {
	BfWriteInstance->WriteShort(entityIndex);
}

void BfWriteType::WriteFloat(float floatValue) {
	BfWriteInstance->WriteFloat(floatValue);
}

void BfWriteType::WriteCoord(float coord) {
	BfWriteInstance->WriteBitCoord(coord);
}

void BfWriteType::WriteNum(int intValue) {
	BfWriteInstance->WriteLong(intValue);
}