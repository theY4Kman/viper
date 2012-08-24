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
					void WriteAngle(float num, int numBits = 8);
					void WriteAngles(boost::python::list anglesList);
					void WriteBool(bool boolValue);
					void WriteByte(char byte);
					void WriteCoord(float coord);
					boost::python::object WriteChar(char charValue);
					boost::python::object WriteEntity(int entityIndex);
					boost::python::object WriteFloat(float floatValue);
					boost::python::object WriteNum(int intValue);
					boost::python::object WriteShort(short shortValue);
					boost::python::object WriteString(std::string stringValue);
					boost::python::object WriteVecCoord(boost::python::list vecCoord);
					boost::python::object WriteVecNormal(boost::python::list vecNormal);
					boost::python::object WriteWord(short word);

				private:
					std::vector<BitBufCell *> Cells;
			};
		}
	}
}

#endif