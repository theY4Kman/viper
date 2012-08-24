#include "BitBufVectorException.h"
#include "Util.h"

namespace py = boost::python;

namespace Viper {
	namespace Modules {
		namespace BitBuf {
			BitBufVectorException::BitBufVectorException(boost::python::tuple badVecList) {
				BadVecList = badVecList;
			}

			std::string BitBufVectorException::what() const {
				char error[256];
				UTIL_Format(error, sizeof(error), "Unable to convert tuple to vector (wrong size, %d != 3)", py::len(BadVecTuple));
				
				return std::string(error);
			}
		}
	}
}