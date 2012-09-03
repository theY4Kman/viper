#ifndef __INCLUDE_DATATYPESMODULE_H__
#define __INCLUDE_DATATYPESMODULE_H__

#include "Macros.h"
#include "BoostPythonSM.h"

extern "C" __declspec(dllexport) void initDatatypes();
extern void destroyDatatypes();
extern void unloadThreadStateDatatypes(PyThreadState *threadState);

#endif