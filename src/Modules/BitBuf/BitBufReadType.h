#ifndef __INCLUDE_BITBUFREADTYPE_H__
#define __INCLUDE_BITBUFREADTYPE_H__

#include "BoostPythonSM.h"
#include "Macros.h"
#include "STL.h"
#include "BitBufCell.h"
#include "Modules\Datatypes\VectorType.h"
#include "Modules\Entity\EntityType.h"

namespace Viper {
	namespace Modules {
		namespace BitBuf {
			class BitBufReadType {
			public:
				BitBufReadType(bf_read *bfRead);

				float ReadAngle(int numBits = 8);
				Viper::Modules::Datatypes::VectorType ReadAngles();
				std::string StrMagic();
				bool ReadBool();
				unsigned char ReadByte();
				char ReadChar();
				float ReadCoord();
				Viper::Modules::Entity::EntityType ReadEntity();
				float ReadFloat();
				int ReadNum();
				short ReadShort();
				std::string ReadString(bool lines = false);
				short ReadWord();
				Viper::Modules::Datatypes::VectorType ReadVecCoord();
				Viper::Modules::Datatypes::VectorType ReadVecNormal();


			private:
				bf_read *BfRead;
			};
		}
	}
}

#endif