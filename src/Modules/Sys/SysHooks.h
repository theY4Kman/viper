#ifndef __INCLUDE_SYSHOOKS_H__
#define __INCLUDE_SYSHOOKS_H__

#include "Macros.h"

namespace Viper {
	namespace Modules {
		namespace Sys {
			extern "C" __declspec(dllexport) void initsyshooks();
		}
	}
}

#endif