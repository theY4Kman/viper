#ifndef __INCLUDE_BITBUFMODULE_H__
#define __INCLUDE_BITBUFMODULE_H__

#include "Macros.h"
#include "BoostPythonSM.h"

extern "C" __declspec(dllexport) void initBitBuf();
extern void destroyBitBuf();
extern void unloadThreadStateBitBuf(PyThreadState *threadState);

#endif