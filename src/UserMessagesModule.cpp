#include "BoostPythonSM.h"
#include "UserMessagesModule.h"
#include "sdk/smsdk_ext.h"
#include "Macros.h"
#include "ViperExtension.h"

namespace py = boost::python;

BOOST_PYTHON_MODULE(entity) {
	py::class_<UserMessageType>("UserMessage", py::no_init)
		.def("start", &UserMessageType::Start)
		.def("end", &UserMessageType::End);

	py::def("get_max_entities", entity__get_max_entities);
	py::def("get_entity_count", entity__get_entity_count);
	py::def("is_entity_networkable", entity__is_entity_networkable);
	py::def("create_edict", entity__create_edict);
}