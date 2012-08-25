#include "CompatWrappers.h"

#if SOURCE_ENGINE >= SE_ORANGEBOX
bool IsFlagSet(ConCommandBase *cmd, int flag) {
	return cmd->IsFlagSet(flag);
}

void InsertServerCommand(const char *buf) {
	engine->InsertServerCommand(buf);
}

ConCommandBase *FindCommandBase(const char *name) {
	return icvar->FindCommandBase(name);
}

ConCommand *FindCommand(const char *name) {
	return icvar->FindCommand(name);
}
#else
bool IsFlagSet(ConCommandBase *cmd, int flag) {
	return cmd->IsBitSet(flag);
}

void InsertServerCommand(const char *buf) {
	engine->InsertServerCommand(buf);
}

ConCommandBase *FindCommandBase(const char *name) {
	ConCommandBase *pBase = icvar->GetCommands();

	while(pBase) {
		if (strcmp(pBase->GetName(), name) == 0) {
			if (!pBase->IsCommand()) {
				return NULL;
			}

			return pBase;
		}
		pBase = const_cast<ConCommandBase *>(pBase->GetNext());
	}

	return NULL;
}

ConCommand *FindCommand(const char *name) {
	ConCommandBase *pBase = icvar->GetCommands();
		
	while (pBase) {
		if (strcmp(pBase->GetName(), name) == 0) {
			if (!pBase->IsCommand()) {
				return NULL;
			}

			return static_cast<ConCommand *>(pBase);
		}
		pBase = const_cast<ConCommandBase *>(pBase->GetNext());
	}
		
	return NULL;
}
#endif //SOURCE_ENGINE >= SE_ORANGEBOX

#if SOURCE_ENGINE >= SE_LEFT4DEAD
int IndexOfEdict(const edict_t *pEdict) {
	return (int)(pEdict - gpGlobals->pEdicts);
}
	
edict_t *PEntityOfEntIndex(int iEntIndex) {
	if (iEntIndex >= 0 && iEntIndex < gpGlobals->maxEntities) {
		return (edict_t *)(gpGlobals->pEdicts + iEntIndex);
	}

	return NULL;
}
	
int GetTypeDescOffs(typedescription_t *td) {
	return td->fieldOffset;
}
#else
int IndexOfEdict(const edict_t *pEdict) {
	return engine->IndexOfEdict(pEdict);
}
	
edict_t *PEntityOfEntIndex(int iEntIndex) {
	return engine->PEntityOfEntIndex(iEntIndex);
}
	
int GetTypeDescOffs(typedescription_t *td) {
	return td->fieldOffset[TD_OFFSET_NORMAL];
}
#endif //SOURCE_ENGINE >= SE_LEFT4DEAD