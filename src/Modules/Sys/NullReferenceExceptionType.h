#ifndef __INCLUDE_NULLREFERENCEEXCEPTIONTYPE_H__
#define __INCLUDE_NULLREFERENCEEXCEPTIONTYPE_H__

#include "STL.h"

namespace Viper {
	namespace Modules {
		namespace Sys {
			class NullReferenceExceptionType {
			public:
				std::string what() const;
			};
		}
	}
}

#endif