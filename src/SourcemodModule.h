#ifndef __INCLUDE_SOURCEMODMODULE_H__
#define __INCLUDE_SOURCEMODMODULE_H__

#include "LoadExtensionException.h"
#include "Macros.h"

extern "C" __declspec(dllexport) void initSourcemod();
extern void destroySourcemod();

DEFINE_CUSTOM_EXCEPTION_DECL(LoadExtensionException, Sourcemod)

#endif