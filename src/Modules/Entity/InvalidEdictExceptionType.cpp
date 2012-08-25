#include "InvalidEdictExceptionType.h"
#include "Util.h"

namespace Viper {
	namespace Modules {
		namespace Entity {
			InvalidEdictExceptionType::InvalidEdictExceptionType(edict_t *invalidEdict) {
				InvalidEdict = invalidEdict;
			}

			std::string InvalidEdictExceptionType::what() const {
				char str[256];
				UTIL_Format(str, sizeof(str), "Invalid edict: %p", InvalidEdict);

				return std::string(str);
			}
		}
	}
}