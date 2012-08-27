#ifndef __INCLUDE_ENTITYTYPE_H__
#define __INCLUDE_ENTITYTYPE_H__

#include "Macros.h"
#include "ViperExtension.h"
#include "HL2SDK.h"
#include "EdictType.h"

class EntityType {
public:
	EntityType(CBaseEntity *baseEntity);

	bool IsValid();
	unsigned int GetSendPropOffs(std::string prop);
	EdictType GetEdict();

private:
	CBaseEntity *BaseEntity;
};

#endif