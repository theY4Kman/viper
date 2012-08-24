#ifndef __INCLUDE_BITBUFTYPE_H__
#define __INCLUDE_BITBUFTYPE_H__

#include "BoostPythonSM.h"
#include "Macros.h"
#include "STL.h"
#include "BitBufCell.h"

namespace Viper {
	namespace Modules {
		namespace BitBuf {
			class BitBufType {
				public:
					~BitBufType();

					void WriteAngle(float num, int numBits = 8);
					void WriteAngles(boost::python::list anglesList);
					void WriteBool(bool boolValue);
					void WriteByte(char byte);
					void WriteCoord(float coord);
					void WriteChar(char charValue);
					void WriteEntity(int entityIndex);
					void WriteFloat(float floatValue);
					void WriteNum(int intValue);
					void WriteShort(short shortValue);
					void WriteString(std::string stringValue);
					void WriteVecCoord(boost::python::list vecCoord);
					void WriteVecNormal(boost::python::list vecNormal);
					void WriteWord(short word);
					boost::python::object StrMagic();

				private:
					std::vector<BitBufCell *> Cells;
			};
		}
	}
}

#endif