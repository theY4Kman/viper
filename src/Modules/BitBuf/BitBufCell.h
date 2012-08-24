#ifndef __INCLUDE_BITBUFCELL_H__
#define __INCLUDE_BITBUFCELL_H__

#include "BitBufTypes.h"

namespace Viper {
	namespace Modules {
		namespace BitBuf {
			struct BitBufCell {
				BitBufTypes type;
	
				union BitBufCellData {
					bool asBool; // Bool
					int asInt; //Char, Byte, Short, Entity, Word
					long asLong; // Num
					float asFloat; // Float, Coord
					char const *asString; // String
					float asVector[3]; // VecCoord, VecNormal
		
					struct {
						float value;
						int numbits;
					} asAngle;
				} data;
			};
		}
	}
}

#endif