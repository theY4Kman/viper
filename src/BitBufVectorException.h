#ifndef __INCLUDE_BITBUFVECTOREXCEPTION_H__
#define __INCLUDE_BITBUFVECTOREXCEPTION_H__

#include "STL.h"
#include "BoostPythonSM.h"

class BitBufVectorException {
public:
	BitBufVectorException(boost::python::list badVecList);

	std::string what() const;

private:
	boost::python::list BadVecList;
};

#endif