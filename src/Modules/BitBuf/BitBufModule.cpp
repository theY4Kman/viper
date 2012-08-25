#include "BoostPythonSM.h"
#include "BitBufModule.h"
#include "BitBufTypes.h"
#include "BitBufReadType.h"
#include "BitBufType.h"
#include "BitBufVectorException.h"

namespace py = boost::python;

namespace Viper {
	namespace Modules {
		namespace BitBuf {
			DEFINE_CUSTOM_EXCEPTION_INIT(BitBufVectorException, bitbuf)

			BOOST_PYTHON_MODULE(bitbuf) {
				py::class_<BitBufType>("BitBuf", py::init<>())
					.def("write_angle", &BitBufType::WriteAngle, (py::arg("num"), py::arg("numBits")=8))
					.def("write_angles", &BitBufType::WriteAngles, (py::arg("angles")))
					.def("write_byte", &BitBufType::WriteByte, (py::arg("byte")))
					.def("write_bool", &BitBufType::WriteBool, (py::arg("bit")))
					.def("write_char", &BitBufType::WriteChar, (py::arg("char")))
					.def("write_coord", &BitBufType::WriteCoord, (py::arg("coord")))
					.def("write_entity", &BitBufType::WriteEntity, (py::arg("entity")))
					.def("write_float", &BitBufType::WriteFloat, (py::arg("num")))
					.def("write_short", &BitBufType::WriteShort, (py::arg("short")))
					.def("write_vec_coord", &BitBufType::WriteVecCoord, (py::arg("vec")))
					.def("write_vec_normal", &BitBufType::WriteVecNormal, (py::arg("vec")))
					.def("write_word", &BitBufType::WriteWord, (py::arg("word")))
					.def("write_string", &BitBufType::WriteString, (py::arg("string")))
					.def("write_num", &BitBufType::WriteNum, (py::arg("num")))
					.def("__str__", &BitBufType::StrMagic);

				py::class_<BitBufReadType>("BitBufRead", py::no_init)
					.def("read_angle", &BitBufReadType::ReadAngle, (py::arg("numBits")=8))
					.def("read_angles", &BitBufReadType::ReadAngles)
					.def("__str__", &BitBufReadType::StrMagic);

				DEFINE_CUSTOM_EXCEPTION(BitBufVectorException, bitbuf,
					PyExc_Exception, "bitbuf.BitBufVectorException",
					"BitBufVectorException")
			}
		}
	}
}