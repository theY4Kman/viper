#ifndef __INCLUDE_SOURCEMODMODULE_H__
#define __INCLUDE_SOURCEMODMODULE_H__

#include "LoadExtensionException.h"
#include "Macros.h"

extern "C" __declspec(dllexport) void initsourcemod();
DEFINE_CUSTOM_EXCEPTION_DECL(LoadExtensionException, sourcemod)

#endif