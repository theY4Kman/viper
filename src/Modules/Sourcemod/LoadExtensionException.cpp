#include "LoadExtensionException.h"

namespace Viper {
	namespace Modules {
		namespace Sourcemod {
			LoadExtensionException::LoadExtensionException(
				std::string fullExtensionName, std::string error) {
				FullExtensionName = fullExtensionName;
				Error = error;
			}

			std::string LoadExtensionException::what() const {
				return "Unable to load extension " + FullExtensionName + ": " +
					Error;
			}
		}
	}
}