#ifndef __INCLUDE_BFWRITETYPE_H__
#define __INCLUDE_BFWRITETYPE_H__

#include "STL.h"
#include "HL2SDK.h"
#include "VectorType.h"

class BfWriteType {
	public:
		BfWriteType(bf_write *bfWriteInstance);

		void WriteAngle(float num, int numBits = 8);
		void WriteAngles(VectorType anglesList);
		void WriteBool(bool boolValue);
		void WriteByte(int byte);
		void WriteCoord(float coord);
		void WriteChar(char charValue);
		void WriteEntity(int entityIndex);
		void WriteFloat(float floatValue);
		void WriteNum(int intValue);
		void WriteShort(int shortValue);
		void WriteString(std::string stringValue);
		void WriteVecCoord(VectorType vecCoord);
		void WriteVecNormal(VectorType vecNormal);
		void WriteWord(int word);

	private:
		bf_write *BfWriteInstance;
};

#endif