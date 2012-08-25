#ifndef __INCLUDE_COMPATWRAPPERS_H__
#define __INCLUDE_COMPATWRAPPERS_H__

#include "Extension.h"
#include "HL2SDK.h"

#if SOURCE_ENGINE >= SE_ORANGEBOX
#define CONVAR_REGISTER(object)				ConVar_Register(0, object)

bool IsFlagSet(ConCommandBase *cmd, int flag);
void InsertServerCommand(const char *buf);
ConCommandBase *FindCommandBase(const char *name);
ConCommand *FindCommand(const char *name);
#else
class CCommand {
public:
	inline const char *ArgS() const {
		return engine->Cmd_Args();
	}
	inline int ArgC() const {
		return engine->Cmd_Argc();
	}
	inline const char *Arg(int index) const {
		return engine->Cmd_Argv(index);
	}
};

bool IsFlagSet(ConCommandBase *cmd, int flag);
void InsertServerCommand(const char *buf);
ConCommandBase *FindCommandBase(const char *name);
ConCommand *FindCommand(const char *name);

#define CVAR_INTERFACE_VERSION				VENGINE_CVAR_INTERFACE_VERSION

#define CONVAR_REGISTER(object)				ConCommandBaseMgr::OneTimeInit(object)
typedef FnChangeCallback					FnChangeCallback_t;
#endif //SOURCE_ENGINE >= SE_ORANGEBOX

#if SOURCE_ENGINE >= SE_LEFT4DEAD
int IndexOfEdict(const edict_t *pEdict);
edict_t *PEntityOfEntIndex(int iEntIndex);
int GetTypeDescOffs(typedescription_t *td);
#else
int IndexOfEdict(const edict_t *pEdict);
edict_t *PEntityOfEntIndex(int iEntIndex);
int GetTypeDescOffs(typedescription_t *td);
#endif //SOURCE_ENGINE >= SE_LEFT4DEAD

#endif //_INCLUDE_SOURCEMOD_COMPAT_WRAPPERS_H_
