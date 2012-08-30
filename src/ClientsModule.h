#ifndef __INCLUDE_CLIENTSMODULE_H__
#define __INCLUDE_CLIENTSMODULE_H__

#include "Macros.h"
#include "STL.h"

extern "C" __declspec(dllexport) void initclients();
extern void destroyclients();

DEFINE_CUSTOM_EXCEPTION_DECL(ClientIndexOutOfRangeExceptionType, clients)
DEFINE_CUSTOM_EXCEPTION_DECL(ClientDataNotAvailableExceptionType, clients)
DEFINE_CUSTOM_EXCEPTION_DECL(ClientNotConnectedExceptionType, clients)
DEFINE_CUSTOM_EXCEPTION_DECL(ClientNotInGameExceptionType, clients)
DEFINE_CUSTOM_EXCEPTION_DECL(ClientNotFakeExceptionType, clients)
DEFINE_CUSTOM_EXCEPTION_DECL(InvalidUserIdExceptionType, clients)
DEFINE_CUSTOM_EXCEPTION_DECL(InvalidClientSerialExceptionType, clients)

extern void clients__GameFrame(bool simulating);

extern std::map<int, std::string> clients__KickQueue;

#endif