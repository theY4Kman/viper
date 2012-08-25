#include "BoostPythonSM.h"
#include "DatatypesModule.h"
#include "VectorType.h"
#include "sdk/smsdk_ext.h"
#include "Macros.h"
#include "ColorType.h"

namespace py = boost::python;

namespace Viper {
	namespace Modules {
		namespace Datatypes {
			BOOST_PYTHON_MODULE(datatypes) {
				py::class_<VectorType>("Vector", py::init<float, float, float>((py::arg("x") = 0.0f, py::arg("y") = 0.0f, py::arg("z") = 0.0f)))
					.def(py::init<VectorType>((py::arg("v"))))
					.def_readwrite("x", &VectorType::X)
					.def_readwrite("y", &VectorType::Y)
					.def_readwrite("z", &VectorType::Z)
					.def("__cmp__", &VectorType::Compare)
					.def("__eq__", &VectorType::CompareEqual)
					.def("__ne__", &VectorType::CompareNotEqual)
					.def("__lt__", &VectorType::CompareLessThan)
					.def("__gt__", &VectorType::CompareGreaterThan)
					.def("__le__", &VectorType::CompareLessThanOrEqual)
					.def("__ge__", &VectorType::CompareGreaterThanOrEqual)
					.def("__repr__", &VectorType::ReprMagic);

				py::class_<ColorType>("Color", py::init<unsigned char, unsigned char, unsigned char, unsigned char>((py::arg("r") = 255, py::arg("g") = 255, py::arg("b") = 255, py::arg("a"))))
					.def(py::init<ColorType>((py::arg("c"))))
					.def_readwrite("r", &ColorType::Red)
					.def_readwrite("g", &ColorType::Green)
					.def_readwrite("b", &ColorType::Blue)
					.def_readwrite("a", &ColorType::Alpha)
					.def("__cmp__", &ColorType::Compare)
					.def("__eq__", &ColorType::CompareEqual)
					.def("__ne__", &ColorType::CompareNotEqual)
					.def("__lt__", &ColorType::CompareLessThan)
					.def("__gt__", &ColorType::CompareGreaterThan)
					.def("__le__", &ColorType::CompareLessThanOrEqual)
					.def("__ge__", &ColorType::CompareGreaterThanOrEqual)
					.def("__repr__", &ColorType::ReprMagic);
			}
		}
	}
}