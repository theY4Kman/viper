#ifndef __INCLUDE_ENTITYOUTPUTCLASSNAMEHOOK_H__
#define __INCLUDE_ENTITYOUTPUTCLASSNAMEHOOK_H__

#include "Macros.h"
#include "HL2SDK.h"

class EntityOutputClassNameHook {
public:
	EntityOutputClassNameHook(std::string className, std::string output, PyThreadState *threadState, boost::python::object callbackFunction);

	std::string ClassName;
	std::string Output;
	PyThreadState *ThreadState;
	boost::python::object CallbackFunction;
};

#endif