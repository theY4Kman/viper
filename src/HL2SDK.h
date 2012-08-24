#ifndef __INCLUDE_HL2SDK_H__
#define __INCLUDE_HL2SDK_H__

#ifdef CLIENT_DLL
#undef CLIENT_DLL
#endif // CLIENT_DLL

#ifndef GAME_DLL
#define GAME_DLL 1
#endif // !GAME_DLL

#include "vstdlib/random.h"
#include "igameevents.h"
#include "IEngineSound.h"
#include "filesystem.h"
#include "icvar.h"

#endif // !__INCLUDE_HL2SDK_H__