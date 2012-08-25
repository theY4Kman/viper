#include "NullReferenceExceptionType.h"

namespace Viper {
	namespace Modules {
		namespace Sys {
			std::string NullReferenceExceptionType::what() const {
				return "NULL Reference";
			}
		}
	}
}