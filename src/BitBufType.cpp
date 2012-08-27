#include "BitBufType.h"
#include "Macros.h"
#include "BoostPythonSM.h"
#include "BitBufVectorException.h"
#include "Util.h"

namespace py = boost::python;

BitBufType::~BitBufType() {
	std::vector<BitBufCell*>::iterator iter = Cells.begin();
				
	while(iter != Cells.end()) {
		BitBufCell *cell = (*iter);
		if (cell->type == BitBufType_String)
			delete [] cell->data.asString;
     
		iter++;   
		delete cell;
	}
}

void BitBufType::WriteAngle(float num, int numBits) {
	BitBufCell *cell = new BitBufCell();
	cell->type = BitBufType_Angle;
	cell->data.asAngle.value = num;
	cell->data.asAngle.numbits = numBits;
	Cells.push_back(cell);
}

void BitBufType::WriteAngles(py::list anglesList) {
	if(py::len(anglesList) != 3) {
		throw BitBufVectorException(anglesList);
	}

	BitBufCell *cell = new BitBufCell();
	cell->type = BitBufType_Angles;

	cell->data.asVector[0] = py::extract<float>(anglesList[0]);
	cell->data.asVector[1] = py::extract<float>(anglesList[1]);
	cell->data.asVector[2] = py::extract<float>(anglesList[2]);
   
	Cells.push_back(cell);
}

void BitBufType::WriteBool(bool boolValue) {
	BitBufCell *cell = new BitBufCell();
	cell->type = BitBufType_Bool;
	cell->data.asBool = boolValue;
				
	Cells.push_back(cell);
}

void BitBufType::WriteByte(char byte) {			
	BitBufCell *cell = new BitBufCell();
	cell->type = BitBufType_Byte;
	cell->data.asInt = byte;
				
	Cells.push_back(cell);
}

void BitBufType::WriteChar(char charValue) {			
	BitBufCell *cell = new BitBufCell();
	cell->type = BitBufType_Char;
	cell->data.asInt = charValue;
				
	Cells.push_back(cell);
}

void BitBufType::WriteShort(short shortValue) {
	BitBufCell *cell = new BitBufCell();
	cell->type = BitBufType_Short;
	cell->data.asInt = shortValue;
    
	Cells.push_back(cell);
}

void BitBufType::WriteString(std::string stringValue) {
	BitBufCell *cell = new BitBufCell();
	cell->type = BitBufType_String;
	cell->data.asString = UTIL_Strdup(stringValue.c_str());
    
	Cells.push_back(cell);
}
			
void BitBufType::WriteVecCoord(py::list vecCoord) {
	if(py::len(vecCoord) != 3) {
		throw BitBufVectorException(vecCoord);
	}

	BitBufCell *cell = new BitBufCell();
	cell->type = BitBufType_VecCoord;

	cell->data.asVector[0] = py::extract<float>(vecCoord[0]);
	cell->data.asVector[1] = py::extract<float>(vecCoord[1]);
	cell->data.asVector[2] = py::extract<float>(vecCoord[2]);
   
	Cells.push_back(cell);
}
			
void BitBufType::WriteVecNormal(boost::python::list vecNormal) {
	if(py::len(vecNormal) != 3) {
		throw BitBufVectorException(vecNormal);
	}

	BitBufCell *cell = new BitBufCell();
	cell->type = BitBufType_VecNormal;

	cell->data.asVector[0] = py::extract<float>(vecNormal[0]);
	cell->data.asVector[1] = py::extract<float>(vecNormal[1]);
	cell->data.asVector[2] = py::extract<float>(vecNormal[2]);
   
	Cells.push_back(cell);
}

void BitBufType::WriteWord(short word) {
	BitBufCell *cell = new BitBufCell();
	cell->type = BitBufType_Word;
	cell->data.asInt = word;
    
	Cells.push_back(cell);
}

void BitBufType::WriteEntity(int entityIndex) {
	BitBufCell *cell = new BitBufCell();
	cell->type = BitBufType_Entity;
	cell->data.asInt = entityIndex;
    
	Cells.push_back(cell);
}

void BitBufType::WriteFloat(float floatValue) {
	BitBufCell *cell = new BitBufCell();
	cell->type = BitBufType_Float;
	cell->data.asFloat = floatValue;
    
	Cells.push_back(cell);
}

void BitBufType::WriteCoord(float coord) {
	BitBufCell *cell = new BitBufCell();
	cell->type = BitBufType_Coord;
	cell->data.asFloat = coord;
    
	Cells.push_back(cell);
}

void BitBufType::WriteNum(int intValue) {
	BitBufCell *cell = new BitBufCell();
	cell->type = BitBufType_Num;
	cell->data.asInt = intValue;
    
	Cells.push_back(cell);
}

py::object BitBufType::StrMagic() {
	char str[256];
	UTIL_Format(str, sizeof(str), "<BitBuf object at: %p>", this);

	return py::str(str);
}