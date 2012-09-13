#ifndef __INCLUDE_SDKTOOLSMODULE_H__
#define __INCLUDE_SDKTOOLSMODULE_H__

#include "Macros.h"
#include "BoostPythonSM.h"
#include "IBinTools.h"

#define SIZEOF_VARIANT_T		20

extern "C" __declspec(dllexport) void initSDKTools();
void destroySDKTools();
extern void unloadThreadStateSDKTools(PyThreadState *threadState);

extern SourceMod::ICallWrapper *sdktools__AcceptEntityInputCallWrapper;
extern unsigned char sdktools__VariantTInstance[SIZEOF_VARIANT_T];

DEFINE_CUSTOM_EXCEPTION_DECL(IServerNotFoundExceptionType, SDKTools)
DEFINE_CUSTOM_EXCEPTION_DECL(LightStyleOutOfRangeExceptionType, SDKTools)
DEFINE_CUSTOM_EXCEPTION_DECL(SDKToolsModSupportNotAvailableExceptionType, SDKTools)
DEFINE_CUSTOM_EXCEPTION_DECL(MapMustBeRunningExceptionType, SDKTools)

#endif