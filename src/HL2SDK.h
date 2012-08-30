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
#include "edict.h"
#include "shareddefs.h"

#if SOURCE_ENGINE >= SE_LEFT4DEAD
#   include "convar_sm_l4d.h"
#elif SOURCE_ENGINE >= SE_ORANGEBOX
#   include "convar_sm_ob.h"
#else
#   include "convar_sm.h"
#endif

#include "IEntityFactoryDictionary.h"

#endif // !__INCLUDE_HL2SDK_H__