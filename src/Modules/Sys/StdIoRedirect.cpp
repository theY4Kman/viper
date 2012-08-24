#include "StdIoRedirect.h"
#include "Extension.h"

namespace Viper {
	namespace Modules {
		namespace Sys {
			void StdIoRedirect::Write(std::string const& str) {
				g_SMAPI->ConPrint(str.c_str());
			}

			void StdIoRedirect::Flush() {

			}
		}
	}
}