#include "BoostPythonSM.h"
#include "DatatypesModule.h"
#include "VectorType.h"
#include "sdk/smsdk_ext.h"
#include "Macros.h"
#include "VectorType.h"

namespace py = boost::python;

namespace Viper {
	namespace Modules {
		namespace Datatypes {
			BOOST_PYTHON_MODULE(datatypes) {
				py::class_<VectorType>("Vector", py::init<>())
					.def_readwrite("x", &VectorType::x)
					.def_readwrite("y", &VectorType::y)
					.def_readwrite("z", &VectorType::z)
					.def("__cmp__", &VectorType::Compare)
					.def("__eq__", &VectorType::CompareEqual)
					.def("__ne__", &VectorType::CompareNotEqual)
					.def("__lt__", &VectorType::CompareLessThan)
					.def("__gt__", &VectorType::CompareGreaterThan)
					.def("__le__", &VectorType::CompareLessThanOrEqual)
					.def("__ge__", &VectorType::CompareGreaterThanOrEqual)
					.def("__repr__", &VectorType::ReprMagic);
			}
		}
	}
}