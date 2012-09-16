#ifndef __INCLUDE_VIPERTRACEFILTER_H__
#define __INCLUDE_VIPERTRACEFILTER_H__

#include "STL.h"
#include "HL2SDK.h"
#include "BoostPythonSM.h"

class ViperTraceFilter : public CTraceFilter {
public:
	ViperTraceFilter(PyThreadState *threadState, boost::python::object callbackFunction);

	bool ShouldHitEntity(IHandleEntity *handleEntity, int contentsMask);
	
private:
	PyThreadState *ThreadState;
	boost::python::object CallbackFunction;
};

#endif