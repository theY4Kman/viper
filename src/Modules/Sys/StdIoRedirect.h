#ifndef __INCLUDE_STDIOREDIRECT_H__
#define __INCLUDE_STDIOREDIRECT_H__

#include "STL.h"

namespace Viper {
	namespace Modules {
		namespace Sys {
			class StdIoRedirect {
			public:
				void Write(std::string const& str);
				void Flush();
			private:
			};
		}
	}
}

#endif