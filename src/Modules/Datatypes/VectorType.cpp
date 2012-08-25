#include "BoostPythonSM.h"
#include "VectorType.h"
#include "sdk/smsdk_ext.h"
#include "Macros.h"
#include "Util.h"

namespace py = boost::python;

namespace Viper {
	namespace Modules {
		namespace Datatypes {
			VectorType::VectorType(float x, float y, float z) {
				X = x;
				Y = y;
				Z = z;
			}

			VectorType::VectorType(const VectorType &v) {
				X = v.X;
				Y = v.Y;
				Z = v.Z;
			}

			bool VectorType::CompareEqual(VectorType v) {
				return X == v.X &&
					Y == v.Y &&
					Z == v.Z;
			}

			bool VectorType::CompareNotEqual(VectorType v) {
				return !CompareEqual(v);
			}

			int VectorType::Compare(VectorType v) {
				return CompareEqual(v) ? 0 :
					(CompareLessThan(v) ? -1 : 1);
			}

			bool VectorType::CompareGreaterThan(VectorType v) {
				return X > v.X &&
					Y > v.Y &&
					Z > v.Z;
			}

			bool VectorType::CompareLessThan(VectorType v) {
				return X < v.X &&
					Y < v.Y &&
					Z < v.Z;
			}

			bool VectorType::CompareGreaterThanOrEqual(VectorType v) {
				return X >= v.X &&
					Y >= v.Y &&
					Z >= v.Z;
			}

			bool VectorType::CompareLessThanOrEqual(VectorType v) {
				return X <= v.X &&
					Y <= v.Y &&
					Z <= v.Z;
			}

			std::string VectorType::ReprMagic() {
				char repr[256];
				UTIL_Format(repr, sizeof(repr), "Vector(%f, %f, %f)", X, Y, Z);
    
				return std::string(repr);
			}
		}
	}
}