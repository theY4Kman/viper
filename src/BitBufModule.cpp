#include "BoostPythonSM.h"
#include "BitBufModule.h"
#include "BfReadType.h"
#include "BfWriteType.h"

namespace py = boost::python;

BOOST_PYTHON_MODULE(BitBuf) {
	py::class_<BfWriteType>("BfWrite", py::no_init)
		.def("WriteAngle", &BfWriteType::WriteAngle, (py::arg("num"), py::arg("numBits")=8))
		.def("WriteAngles", &BfWriteType::WriteAngles, (py::arg("angles")))
		.def("WriteByte", &BfWriteType::WriteByte, (py::arg("byte")))
		.def("WriteBool", &BfWriteType::WriteBool, (py::arg("bit")))
		.def("WriteChar", &BfWriteType::WriteChar, (py::arg("char")))
		.def("WriteCoord", &BfWriteType::WriteCoord, (py::arg("coord")))
		.def("WriteEntity", &BfWriteType::WriteEntity, (py::arg("entity")))
		.def("WriteFloat", &BfWriteType::WriteFloat, (py::arg("num")))
		.def("WriteShort", &BfWriteType::WriteShort, (py::arg("short")))
		.def("WriteVecCoord", &BfWriteType::WriteVecCoord, (py::arg("vec")))
		.def("WriteVecNormal", &BfWriteType::WriteVecNormal, (py::arg("vec")))
		.def("WriteWord", &BfWriteType::WriteWord, (py::arg("word")))
		.def("WriteString", &BfWriteType::WriteString, (py::arg("string")))
		.def("WriteNum", &BfWriteType::WriteNum, (py::arg("num")));

	py::class_<BfReadType>("BfRead", py::no_init)
		.def("ReadAngle", &BfReadType::ReadAngle, (py::arg("numBits")=8))
		.def("ReadAngles", &BfReadType::ReadAngles)
		.def("ReadByte", &BfReadType::ReadByte)
		.def("ReadBool", &BfReadType::ReadBool)
		.def("ReadChar", &BfReadType::ReadChar)
		.def("ReadCoord", &BfReadType::ReadCoord)
		.def("ReadEntity", &BfReadType::ReadEntity)
		.def("ReadFloat", &BfReadType::ReadFloat)
		.def("ReadShort", &BfReadType::ReadShort)
		.def("ReadVecCoord", &BfReadType::ReadVecCoord)
		.def("ReadVecNormal", &BfReadType::ReadVecNormal)
		.def("ReadWord", &BfReadType::ReadWord)
		.def("ReadString", &BfReadType::ReadString)
		.def("ReadNum", &BfReadType::ReadNum);
}

void destroyBitBuf() {
}