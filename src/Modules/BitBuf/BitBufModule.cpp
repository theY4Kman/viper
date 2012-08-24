#include "BoostPythonSM.h"
#include "BitBufModule.h"
#include "BitBufTypes.h"
#include "BitBufType.h"
#include "BitBufVectorException.h"

namespace py = boost::python;

namespace Viper {
	namespace Modules {
		namespace BitBuf {
			DEFINE_CUSTOM_EXCEPTION_INIT(BitBufVectorException, bitbuf)

			BOOST_PYTHON_MODULE(bitbuf) {
				py::class_<BitBufType>("BitBuf", py::init<>())
					.def("write_angle", py::raw_function(&bitbuf__BitBufType_WriteAngle, 1))
					.def("write_angles", py::raw_function(&bitbuf__BitBufType_WriteAngles, 1));

				DEFINE_CUSTOM_EXCEPTION(BitBufVectorException, bitbuf,
					PyExc_Exception, "bitbuf.BitBufVectorException",
					"BitBufVectorException")
			}
		}
	}
}