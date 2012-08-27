#include "BitBufVectorException.h"
#include "Util.h"

namespace py = boost::python;

BitBufVectorException::BitBufVectorException(boost::python::list badVecList) {
	BadVecList = badVecList;
}

std::string BitBufVectorException::what() const {
	char error[256];
	UTIL_Format(error, sizeof(error), "Unable to convert list to vector (wrong size, %d != 3)", py::len(BadVecList));
				
	return std::string(error);
}