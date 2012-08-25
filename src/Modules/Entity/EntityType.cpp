#include "BoostPythonSM.h"
#include "EntityType.h"
#include "Macros.h"
#include "CompatWrappers.h"
#include "Extension.h"

namespace py = boost::python;

namespace Viper {
	namespace Modules {
		namespace Entity {
			bool EntityType::IsValid() {
				if(BaseEntity == NULL) {
					return false;
				}

				edict_t *edict = g_Extension.GetServerGameEnts()->BaseEntityToEdict(BaseEntity);

				if(edict == NULL || edict->IsFree()) {
					return false;
				}

				return true;
			}
		}
	}
}