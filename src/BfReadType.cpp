#include "BfReadType.h"
#include "Macros.h"
#include "Util.h"
#include "NullReferenceExceptionType.h"

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

bool BfReadType::ReadBool() {
	return BfReadInstance->ReadOneBit() ? true : false;
}

unsigned char BfReadType::ReadByte() {
	int byteInt = BfReadInstance->ReadByte();
	return (unsigned char)byteInt;
}

char BfReadType::ReadChar() {
	return BfReadInstance->ReadChar();
}

float BfReadType::ReadCoord() {
	return BfReadInstance->ReadBitCoord();
}

int BfReadType::ReadEntity() {
	return BfReadInstance->ReadShort();
}

float BfReadType::ReadFloat() {
	return BfReadInstance->ReadFloat();
}

int BfReadType::ReadNum() {
	return BfReadInstance->ReadLong();
}

short BfReadType::ReadShort() {
	return BfReadInstance->ReadShort();
}

std::string BfReadType::ReadString(bool lines) {
	char str[1024];
	BfReadInstance->ReadString(str, sizeof(str), lines);

	return std::string(str);
}

short BfReadType::ReadWord() {
	return BfReadInstance->ReadWord();
}

VectorType BfReadType::ReadVecCoord() {
	Vector v;
	BfReadInstance->ReadBitVec3Coord(v);

	return VectorType(v.x, v.y, v.z);
}

VectorType BfReadType::ReadVecNormal() {
	Vector v;
	BfReadInstance->ReadBitVec3Normal(v);

	return VectorType(v.x, v.y, v.z);
}