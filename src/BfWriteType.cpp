#include "BfWriteType.h"
#include "Macros.h"
#include "BoostPythonSM.h"
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

void BfWriteType::WriteAngles(VectorType anglesVec) {
	BfWriteInstance->WriteBitAngles(QAngle(anglesVec.X, anglesVec.Y, anglesVec.Z));
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
			
void BfWriteType::WriteVecCoord(VectorType vecCoord) {
	BfWriteInstance->WriteBitVec3Coord(Vector(vecCoord.X, vecCoord.Y, vecCoord.Z));
}
			
void BfWriteType::WriteVecNormal(VectorType vecNormal) {
	BfWriteInstance->WriteBitVec3Normal(Vector(vecNormal.X, vecNormal.Y, vecNormal.Z));
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