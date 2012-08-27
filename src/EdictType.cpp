#include "EdictType.h"
#include "InvalidEdictExceptionType.h"
#include "Util.h"

EdictType::EdictType(edict_t *edict) {
	if(edict == NULL || edict->IsFree()) {
		throw InvalidEdictExceptionType(edict);
	}

	Edict = edict;
}

bool EdictType::IsValid() {
	return Edict != NULL && !Edict->IsFree();
}

std::string EdictType::StrMagic() {
	char str[256];
	UTIL_Format(str, sizeof(str), "<Edict object at: %p>", this);

	return std::string(str);
}