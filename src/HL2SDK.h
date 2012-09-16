#ifndef __INCLUDE_HL2SDK_H__
#define __INCLUDE_HL2SDK_H__

#ifdef CLIENT_DLL
#undef CLIENT_DLL
#endif // CLIENT_DLL

#ifndef GAME_DLL
#define GAME_DLL 1
#endif // !GAME_DLL

#include <public/vstdlib/random.h>
#include <public/igameevents.h>
#include <public/engine/IEngineSound.h>
#include <public/filesystem.h>
#include <public/icvar.h>
#include <public/edict.h>
#include <public/iserver.h>
#include <game/shared/shareddefs.h>

#if SOURCE_ENGINE >= SE_ORANGEBOX
#include <public/toolframework/itoolentity.h>
#endif

#if SOURCE_ENGINE >= SE_LEFT4DEAD
#   include "convar_sm_l4d.h"
#elif SOURCE_ENGINE >= SE_ORANGEBOX
#   include "convar_sm_ob.h"
#else
#   include "convar_sm.h"
#endif

#include <public\engine\IEngineTrace.h>
#include <public\ivoiceserver.h>

#endif // !__INCLUDE_HL2SDK_H__