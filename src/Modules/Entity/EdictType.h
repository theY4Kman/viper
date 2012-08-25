#ifndef __INCLUDE_EDICTTYPE_H__
#define __INCLUDE_EDICTTYPE_H__

#include "Extension.h"

namespace Viper {
	namespace Modules {
		namespace Entity {
			class EdictType {
			public:
				EdictType(edict_t *edict);
				
				bool IsValid();
				std::string StrMagic();

			private:
				edict_t *Edict;
			};
		}
	}
}

#endif