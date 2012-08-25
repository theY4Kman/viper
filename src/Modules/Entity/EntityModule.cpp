#include "BoostPythonSM.h"
#include "EntityModule.h"
#include "sdk/smsdk_ext.h"
#include "Macros.h"
#include "Extension.h"
#include "EdictType.h"

namespace py = boost::python;

namespace Viper {
	namespace Modules {
		namespace Entity {
			int entity__get_max_entities() {
				return g_Extension.GetGlobals()->maxEntities;
			}

			int entity__get_entity_count() {
				return engine->GetEntityCount();
			}

			bool entity__is_valid_entity(int entityIndex) {
				edict_t *edict = engine->PEntityOfEntIndex(entityIndex);

				if(edict == NULL || edict->IsFree()) {
					return false;
				}

				IServerUnknown *unknown = edict->GetUnknown();

				if(unknown == NULL) {
					return false;
				}

				CBaseEntity *baseEntity = unknown->GetBaseEntity();

				return baseEntity != NULL;
			}

			bool entity__is_valid_edict(int entityIndex) {
				edict_t *edict = engine->PEntityOfEntIndex(entityIndex);

				if(edict == NULL) {
					return false;
				}

				return !edict->IsFree();
			}

			bool entity__is_entity_networkable(int entityIndex) {
				edict_t *edict = engine->PEntityOfEntIndex(entityIndex);

				if(edict == NULL || edict->IsFree()) {
					return false;
				}

				IServerNetworkable *networkable = edict->GetNetworkable();

				return networkable != NULL;
			}

			BOOST_PYTHON_MODULE(entity) {
				py::class_<EdictType>("Edict", py::no_init)
					.def("is_valid", &EdictType::IsValid)
					.def("__str__", &EdictType::StrMagic);

				py::def("get_max_entities", entity__get_max_entities);
				py::def("get_entity_count", entity__get_entity_count);
				py::def("is_valid_entity", entity__is_valid_entity);
				py::def("is_valid_edict", entity__is_valid_edict);
				py::def("is_entity_networkable", entity__is_entity_networkable);
			}
		}
	}
}