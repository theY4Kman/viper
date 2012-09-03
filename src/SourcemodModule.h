#ifndef __INCLUDE_SOURCEMODMODULE_H__
#define __INCLUDE_SOURCEMODMODULE_H__

#include "LoadExtensionException.h"
#include "Macros.h"
#include "BoostPythonSM.h"

extern "C" __declspec(dllexport) void initSourcemod();
extern void destroySourcemod();
extern void unloadThreadStateSourcemod(PyThreadState *threadState);

DEFINE_CUSTOM_EXCEPTION_DECL(LoadExtensionException, Sourcemod)

#endif