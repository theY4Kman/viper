#include "BoostPythonSM.h"
#include "BitBufModule.h"
#include "BfReadType.h"
#include "BfWriteType.h"

namespace py = boost::python;

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

	py::class_<BfReadType>("BfRead", py::no_init)
		.def("read_angle", &BfReadType::ReadAngle, (py::arg("numBits")=8))
		.def("read_angles", &BfReadType::ReadAngles)
		.def("read_byte", &BfReadType::ReadByte)
		.def("read_bool", &BfReadType::ReadBool)
		.def("read_char", &BfReadType::ReadChar)
		.def("read_coord", &BfReadType::ReadCoord)
		.def("read_entity", &BfReadType::ReadEntity)
		.def("read_float", &BfReadType::ReadFloat)
		.def("read_short", &BfReadType::ReadShort)
		.def("read_vec_coord", &BfReadType::ReadVecCoord)
		.def("read_vec_normal", &BfReadType::ReadVecNormal)
		.def("read_word", &BfReadType::ReadWord)
		.def("read_string", &BfReadType::ReadString)
		.def("read_num", &BfReadType::ReadNum);
}