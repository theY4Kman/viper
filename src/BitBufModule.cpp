#include "BoostPythonSM.h"
#include "BitBufModule.h"
#include "BfReadType.h"
#include "BfWriteType.h"
#include "BitBufVectorException.h"

namespace py = boost::python;

DEFINE_CUSTOM_EXCEPTION_INIT(BitBufVectorException, bitbuf)

BOOST_PYTHON_MODULE(bitbuf) {
	py::class_<BfWriteType>("BfWrite", py::no_init)
		.def("write_angle", &BfWriteType::WriteAngle, (py::arg("num"), py::arg("numBits")=8))
		.def("write_angles", &BfWriteType::WriteAngles, (py::arg("angles")))
		.def("write_byte", &BfWriteType::WriteByte, (py::arg("byte")))
		.def("write_bool", &BfWriteType::WriteBool, (py::arg("bit")))
		.def("write_char", &BfWriteType::WriteChar, (py::arg("char")))
		.def("write_coord", &BfWriteType::WriteCoord, (py::arg("coord")))
		.def("write_entity", &BfWriteType::WriteEntity, (py::arg("entity")))
		.def("write_float", &BfWriteType::WriteFloat, (py::arg("num")))
		.def("write_short", &BfWriteType::WriteShort, (py::arg("short")))
		.def("write_vec_coord", &BfWriteType::WriteVecCoord, (py::arg("vec")))
		.def("write_vec_normal", &BfWriteType::WriteVecNormal, (py::arg("vec")))
		.def("write_word", &BfWriteType::WriteWord, (py::arg("word")))
		.def("write_string", &BfWriteType::WriteString, (py::arg("string")))
		.def("write_num", &BfWriteType::WriteNum, (py::arg("num")));

	// TODO: BfRead
	py::class_<BfReadType>("BfRead", py::no_init)
		.def("read_angle", &BfReadType::ReadAngle, (py::arg("numBits")=8))
		.def("read_angles", &BfReadType::ReadAngles);

	DEFINE_CUSTOM_EXCEPTION(BitBufVectorException, bitbuf,
		PyExc_Exception, "bitbuf.BitBufVectorException",
		"BitBufVectorException")
}