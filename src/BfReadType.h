#ifndef __INCLUDE_BFREADTYPE_H__
#define __INCLUDE_BFREADTYPE_H__

#include "STL.h"
#include "HL2SDK.h"
#include "VectorType.h"

class BfReadType {
public:
	BfReadType(bf_read *bfReadInstance);

	float ReadAngle(int numBits = 8);
	VectorType ReadAngles();
	bool ReadBool();
	unsigned char ReadByte();
	char ReadChar();
	float ReadCoord();
	int ReadEntity();
	float ReadFloat();
	int ReadNum();
	short ReadShort();
	std::string ReadString(bool lines = false);
	short ReadWord();
	VectorType ReadVecCoord();
	VectorType ReadVecNormal();

private:
	bf_read *BfReadInstance;
};

#endif