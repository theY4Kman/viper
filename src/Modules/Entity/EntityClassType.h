#ifndef __INCLUDE_ENTITYCLASSTYPE_H__
#define __INCLUDE_ENTITYCLASSTYPE_H__

#include "Macros.h"

namespace Viper {
	namespace Modules {
		namespace Entity {
			class EntityClassType {
			public:
				EntityClassType(std::string className);
				unsigned int GetSendPropOffs(std::string prop);

			private:
				std::string ClassName;
			};
		}
	}
}

#endif