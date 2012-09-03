#ifndef __INCLUDE_FORWARDFUNCTION_H__
#define __INCLUDE_FORWARDFUNCTION_H__

#include "BoostPythonSM.h"

struct ForwardFunction {
	boost::python::object PythonFunction;
	PyThreadState *ThreadState;
};

#endif