#ifndef __INCLUDE_ENTITYTYPE_H__
#define __INCLUDE_ENTITYTYPE_H__

#include "Macros.h"

namespace Viper {
	namespace Modules {
		namespace Entity {
			class EntityType {
			public:
				EntityType(CBaseEntity *baseEntity);
				unsigned int GetSendPropOffs(std::string prop);

			private:
				CBaseEntity *BaseEntity;
			};
		}
	}
}

#endif