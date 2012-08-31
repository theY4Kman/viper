#ifndef __INCLUDE_ENTITYMODULE_H__
#define __INCLUDE_ENTITYMODULE_H__

#include "Macros.h"

extern "C" __declspec(dllexport) void initentity();
extern void destroyentity();

DEFINE_CUSTOM_EXCEPTION_DECL(InvalidEdictExceptionType, entity)
DEFINE_CUSTOM_EXCEPTION_DECL(InvalidEntityExceptionType, entity)
DEFINE_CUSTOM_EXCEPTION_DECL(EntityNotNetworkableExceptionType, entity)
DEFINE_CUSTOM_EXCEPTION_DECL(InvalidSendPropertyExceptionType, entity)
DEFINE_CUSTOM_EXCEPTION_DECL(InvalidDataMapExceptionType, entity)
DEFINE_CUSTOM_EXCEPTION_DECL(InvalidDataMapPropertyExceptionType, entity)
DEFINE_CUSTOM_EXCEPTION_DECL(EntityOffsetOutOfRangeExceptionType, entity)

#endif