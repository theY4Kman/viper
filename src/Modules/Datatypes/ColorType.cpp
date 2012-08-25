#include "BoostPythonSM.h"
#include "ColorType.h"
#include "sdk/smsdk_ext.h"
#include "Macros.h"
#include "Util.h"

namespace py = boost::python;

namespace Viper {
	namespace Modules {
		namespace Datatypes {
			ColorType::ColorType(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
				Red = r;
				Green = g;
				Blue = b;
				Alpha = a;
			}

			ColorType::ColorType(const ColorType &c) {
				Red = c.Red;
				Green = c.Green;
				Blue = c.Blue;
				Alpha = c.Alpha;
			}

			bool ColorType::CompareEqual(ColorType c) {
				return Red == c.Red &&
					Green == c.Green &&
					Blue == c.Blue &&
					Alpha == c.Alpha;
			}

			bool ColorType::CompareNotEqual(ColorType c) {
				return !CompareEqual(c);
			}

			int ColorType::Compare(ColorType c) {
				return CompareEqual(c) ? 0 :
					(CompareLessThan(c) ? -1 : 1);
			}

			bool ColorType::CompareGreaterThan(ColorType c) {
				return Red > c.Red &&
					Green > c.Green &&
					Blue > c.Blue &&
					Alpha > c.Alpha;
			}

			bool ColorType::CompareLessThan(ColorType c) {
				return Red < c.Red &&
					Green < c.Green &&
					Blue < c.Blue &&
					Alpha < c.Alpha;
			}

			bool ColorType::CompareGreaterThanOrEqual(ColorType c) {
				return Red >= c.Red &&
					Green >= c.Green &&
					Blue >= c.Blue &&
					Alpha >= c.Alpha;
			}

			bool ColorType::CompareLessThanOrEqual(ColorType c) {
				return Red <= c.Red &&
					Green <= c.Green &&
					Blue <= c.Blue &&
					Alpha <= c.Alpha;
			}

			std::string ColorType::ReprMagic() {
				char repr[256];
				UTIL_Format(repr, sizeof(repr), "Color(%d, %d, %d, %d)", Red, Green, Blue, Alpha);
    
				return std::string(repr);
			}
		}
	}
}