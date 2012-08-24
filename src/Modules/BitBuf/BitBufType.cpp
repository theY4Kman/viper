#include "BitBufType.h"
#include "Macros.h"
#include "BoostPythonSM.h"
#include "BitBufVectorException.h"

namespace py = boost::python;

namespace Viper {
	namespace Modules {
		namespace BitBuf {
			WRAP_OBJECT_METHOD(BitBufType, bitbuf, WriteAngle)
			WRAP_OBJECT_METHOD(BitBufType, bitbuf, WriteAngles)

			py::object BitBufType::WriteAngle(py::tuple argumentsTuple,
				py::dict keywordsDict) {
				float num = keywordsDict.contains("num") ?
					py::extract<float>(keywordsDict["num"]) :
					(py::len(argumentsTuple) >= 2 ? py::extract<float>(
						argumentsTuple[1]) : 0.0f);

				int numBits = keywordsDict.contains("numBits") ?
					py::extract<int>(keywordsDict["numBits"]) : 8;

				BitBufCell *cell = new BitBufCell();
				cell->type = BitBufType_Angle;
				cell->data.asAngle.value = num;
				cell->data.asAngle.numbits = numBits;
				Cells.push_back(cell);

				return py::object(py::handle<>(py::borrowed(Py_None)));
			}

			py::object BitBufType::WriteAngles(py::tuple argumentsTuple,
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

				return py::object(py::handle<>(py::borrowed(Py_None)));
			}
		}
	}
}