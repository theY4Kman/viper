#ifndef __INCLUDE_BFREADTYPE_H__
#define __INCLUDE_BFREADTYPE_H__

#include "BoostPythonSM.h"
#include "Macros.h"
#include "STL.h"
#include "VectorType.h"
#include "EntityType.h"

class BfReadType {
public:
	BfReadType(bf_read *bfReadInstance);

	float ReadAngle(int numBits = 8);
	VectorType ReadAngles();
	std::string StrMagic();
	bool ReadBool();
	unsigned char ReadByte();
	char ReadChar();
	float ReadCoord();
	EntityType ReadEntity();
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