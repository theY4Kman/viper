#ifndef __INCLUDE_SDKTOOLSMODULE_H__
#define __INCLUDE_SDKTOOLSMODULE_H__

#include "Macros.h"
#include "BoostPythonSM.h"
#include "IBinTools.h"
#include "TempEntHook.h"
#include "HL2SDK.h"

#define SIZEOF_VARIANT_T		20

extern "C" __declspec(dllexport) void initSDKTools();
void destroySDKTools();
extern void unloadThreadStateSDKTools(PyThreadState *threadState);

extern SourceMod::ICallWrapper *sdktools__AcceptEntityInputCallWrapper;
extern unsigned char sdktools__VariantTInstance[SIZEOF_VARIANT_T];

extern SourceMod::ICallWrapper *sdktools__AcceptEntityInputCallWrapper;
extern SourceMod::ICallWrapper *sdktools__GiveNamedItemCallWrapper;
extern SourceMod::ICallWrapper *sdktools__RemovePlayerItemCallWrapper;
extern SourceMod::ICallWrapper *sdktools__IgniteEntityCallWrapper;
extern SourceMod::ICallWrapper *sdktools__ExtinguishEntityCallWrapper;
extern SourceMod::ICallWrapper *sdktools__ForcePlayerSuicideCallWrapper;
extern SourceMod::ICallWrapper *sdktools__TeleportEntityCallWrapper;
extern SourceMod::ICallWrapper *sdktools__GetClientWeaponSlotCallWrapper;
extern SourceMod::ICallWrapper *sdktools__ActivateEntityCallWrapper;
extern SourceMod::ICallWrapper *sdktools__EquipPlayerWeaponCallWrapper;
extern SourceMod::ICallWrapper *sdktools__SetEntityModelCallWrapper;
extern SourceMod::ICallWrapper *sdktools__GetClientEyeAnglesCallWrapper;

#if SOURCE_ENGINE < SE_ORANGEBOX
extern SourceMod::ICallWrapper *sdktools__CreateEntityByNameCallWrapper;
extern SourceMod::ICallWrapper *sdktools__DispatchKeyValueCallWrapper;
extern SourceMod::ICallWrapper *sdktools__DispatchKeyValueFloatCallWrapper;
extern SourceMod::ICallWrapper *sdktools__DispatchKeyValueVectorCallWrapper;
extern SourceMod::ICallWrapper *sdktools__DispatchKeyValueVectorCallWrapper;
extern SourceMod::ICallWrapper *sdktools__DispatchSpawnCallWrapper;
#endif

#define SPEAK_NORMAL		0
#define SPEAK_MUTED			1
#define SPEAK_ALL			2
#define SPEAK_LISTENALL		4
#define SPEAK_TEAM			8
#define SPEAK_LISTENTEAM	16
#define LISTEN_DEFAULT		0
#define LISTEN_NO			1
#define LISTEN_YES			2

/* this enum needs to be in the order as the LISTEN_ defines above to keep
   the old broken SetClientListening functionality untouched */
enum ListenOverride
{
	Listen_Default,		/**< Leave it up to the game */
	Listen_No,			/**< Can't hear */
	Listen_Yes,			/**< Can hear */
};

extern size_t sdktools__VoiceFlags[65];
extern size_t sdktools__VoiceHookCount;
extern ListenOverride sdktools__VoiceMap[65][65];
extern bool sdktools__ClientMutes[65][65];

extern std::list<TempEntHook> sdktools__TEHooks;

extern void *sdktools__TECurrentEffect;

extern bool sdktools__OnSetClientListening(int iReceiver, int iSender, bool bListen);
extern void sdktools__OnClientCommand(edict_t *pEntity, const CCommand &args);

extern bool sdktools__VoiceDecHookCount();
extern void sdktools__VoiceIncHookCount();

extern void sdktools__LoadTempEnts();
extern void sdktools__OnFireOutput(void *pOutput, CBaseEntity *pActivator, CBaseEntity *pCaller, float fDelay);
extern void sdktools__OnCoreMapStart(edict_t *pEdictList, int edictCount, int clientMax);

DEFINE_CUSTOM_EXCEPTION_DECL(IServerNotFoundExceptionType, SDKTools)
DEFINE_CUSTOM_EXCEPTION_DECL(LightStyleOutOfRangeExceptionType, SDKTools)
DEFINE_CUSTOM_EXCEPTION_DECL(SDKToolsModSupportNotAvailableExceptionType, SDKTools)
DEFINE_CUSTOM_EXCEPTION_DECL(MapMustBeRunningExceptionType, SDKTools)
DEFINE_CUSTOM_EXCEPTION_DECL(InvalidStringTableExceptionType, SDKTools)
DEFINE_CUSTOM_EXCEPTION_DECL(InvalidStringTableStringIndexExceptionType, SDKTools)
DEFINE_CUSTOM_EXCEPTION_DECL(InvalidTempEntExceptionType, SDKTools)
DEFINE_CUSTOM_EXCEPTION_DECL(NoTempEntCallInProgressExceptionType, SDKTools)
DEFINE_CUSTOM_EXCEPTION_DECL(InvalidTempEntPropertyExceptionType, SDKTools)
DEFINE_CUSTOM_EXCEPTION_DECL(TempEntHookDoesNotExistExceptionType, SDKTools)
DEFINE_CUSTOM_EXCEPTION_DECL(EntityOutputClassNameHookDoesNotExistExceptionType, SDKTools)
DEFINE_CUSTOM_EXCEPTION_DECL(InvalidTeamExceptionType, SDKTools)

#endif