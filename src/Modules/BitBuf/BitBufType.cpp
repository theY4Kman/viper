#include "BitBufType.h"
#include "Macros.h"
#include "BoostPythonSM.h"
#include "BitBufVectorException.h"

namespace py = boost::python;

namespace Viper {
	namespace Modules {
		namespace BitBuf {
			void BitBufType::WriteAngle(float num, int numBits) {
				BitBufCell *cell = new BitBufCell();
				cell->type = BitBufType_Angle;
				cell->data.asAngle.value = num;
				cell->data.asAngle.numbits = numBits;
				Cells.push_back(cell);
			}

		/*	py::object BitBufType::WriteAngles(py::tuple argumentsTuple,
				py::dict keywordsDict) {
				py::list vecList = py::extract<py::list>(argumentsTuple[1]);

				if(py::len(vecList) != 3) {
					throw BitBufVectorException(vecList);
				}

				BitBufCell *cell = new BitBufCell();
				cell->type = BitBufType_Angles;

				cell->data.asVector[0] = py::extract<float>(vecList[0]);
				cell->data.asVector[1] = py::extract<float>(vecList[1]);
				cell->data.asVector[2] = py::extract<float>(vecList[2]);
   
				Cells.push_back(cell);

				return BOOST_PY_NONE;
			}

			py::object BitBufType::WriteBool(py::tuple argumentsTuple,
				py::dict keywordsDict) {
				bool bit = py::extract<bool>(argumentsTuple[1]);
				
				BitBufCell *cell = new BitBufCell();
				cell->type = BitBufType_Bool;
				cell->data.asBool = bit;
				
				Cells.push_back(cell);

				return BOOST_PY_NONE;
			}

			py::object BitBufType::WriteByte(char byte) {			
				BitBufCell *cell = new BitBufCell();
				cell->type = BitBufType_Byte;
				cell->data.asInt = byte;
				
				Cells.push_back(cell);

				return BOOST_PY_NONE;
			}*/
		}
	}
}