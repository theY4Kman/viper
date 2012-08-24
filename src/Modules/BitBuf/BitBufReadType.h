#ifndef __INCLUDE_BITBUFREADTYPE_H__
#define __INCLUDE_BITBUFREADTYPE_H__

#include "BoostPythonSM.h"
#include "Macros.h"
#include "STL.h"
#include "BitBufCell.h"

namespace Viper {
	namespace Modules {
		namespace BitBuf {
			class BitBufReadType {
				public:
					float ReadAngle(int numBits = 8);

			private:
				bf_read *bfRead;
			};
		}
	}
}

#endif