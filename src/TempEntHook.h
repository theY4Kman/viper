#ifndef __INCLUDE_TEMPENTHOOK_H__
#define __INCLUDE_TEMPENTHOOK_H__

#include "BoostPythonSM.h"
#include "STL.h"

class TempEntHook {
public:
	TempEntHook(std::string effectName, boost::python::object pythonCallback, PyThreadState *threadState);

	std::string EffectName;
	boost::python::object PythonCallback;
	PyThreadState *ThreadState;
};

#endif