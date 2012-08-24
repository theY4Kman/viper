#include "BoostPythonSM.h"
#include "VectorType.h"
#include "sdk/smsdk_ext.h"
#include "Macros.h"
#include "Util.h"

namespace py = boost::python;

namespace Viper {
	namespace Modules {
		namespace Datatypes {
			bool VectorType::CompareEqual(VectorType v) {
				return x == v.x &&
					y == v.y &&
					z == v.z;
			}

			bool VectorType::CompareNotEqual(VectorType v) {
				return !CompareEqual(v);
			}

			int VectorType::Compare(VectorType v) {
				return CompareEqual(v) ? 0 :
					(CompareLessThan(v) ? -1 : 1);
			}

			bool VectorType::CompareGreaterThan(VectorType v) {
				return x > v.x &&
					y > v.y &&
					z > v.z;
			}

			bool VectorType::CompareLessThan(VectorType v) {
				return x < v.x &&
					y < v.y &&
					z < v.z;
			}

			bool VectorType::CompareGreaterThanOrEqual(VectorType v) {
				return x >= v.x &&
					y >= v.y &&
					z >= v.z;
			}

			bool VectorType::CompareLessThanOrEqual(VectorType v) {
				return x <= v.x &&
					y <= v.y &&
					z <= v.z;
			}

			std::string VectorType::ReprMagic() {
				char repr[256];
				UTIL_Format(repr, sizeof(repr), "Vector(%f, %f, %f)", x, y, z);
    
				return std::string(repr);
			}
		}
	}
}