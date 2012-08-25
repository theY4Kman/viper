#include "BitBufReadType.h"
#include "Macros.h"
#include "BoostPythonSM.h"
#include "Util.h"
#include "Modules\Sys\NullReferenceExceptionType.h"

namespace py = boost::python;
namespace dt = Viper::Modules::Datatypes;
namespace sys = Viper::Modules::Sys;

namespace Viper {
	namespace Modules {
		namespace BitBuf {
			BitBufReadType::BitBufReadType(bf_read *bfRead) {
				if(bfRead == NULL) {
					throw sys::NullReferenceExceptionType();
				}

				BfRead = bfRead;
			}

			float BitBufReadType::ReadAngle(int numBits) {
				return BfRead->ReadBitAngle(numBits);
			}

			dt::VectorType BitBufReadType::ReadAngles() {
				QAngle qAngle;
				BfRead->ReadBitAngles(qAngle);

				return dt::VectorType(qAngle.x,	qAngle.y, qAngle.z);
			}

			std::string BitBufReadType::StrMagic() {
				char str[256];
				UTIL_Format(str, sizeof(str), "<BitBufRead object at: %p>", this);

				return std::string(str);
			}
		}
	}
}