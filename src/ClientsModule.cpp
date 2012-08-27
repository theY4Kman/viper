#include "BoostPythonSM.h"
#include "ClientsModule.h"
#include "ClientType.h"

namespace py = boost::python;

BOOST_PYTHON_MODULE(clients) {
	py::class_<ClientType>("Client", py::no_init)
		.def("__str__", &ClientType::StrMagic);
}

void destroyclients() {
}