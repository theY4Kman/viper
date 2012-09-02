#ifndef __INCLUDE_ENTITYMODULE_H__
#define __INCLUDE_ENTITYMODULE_H__

#include "Macros.h"

extern "C" __declspec(dllexport) void initEntity();
extern void destroyEntity();

DEFINE_CUSTOM_EXCEPTION_DECL(InvalidEdictExceptionType, Entity)
DEFINE_CUSTOM_EXCEPTION_DECL(InvalidEntityExceptionType, Entity)
DEFINE_CUSTOM_EXCEPTION_DECL(EntityNotNetworkableExceptionType, Entity)
DEFINE_CUSTOM_EXCEPTION_DECL(InvalidSendPropertyExceptionType, Entity)
DEFINE_CUSTOM_EXCEPTION_DECL(InvalidDataMapExceptionType, Entity)
DEFINE_CUSTOM_EXCEPTION_DECL(InvalidDataMapPropertyExceptionType, Entity)
DEFINE_CUSTOM_EXCEPTION_DECL(EntityOffsetOutOfRangeExceptionType, Entity)

#endif