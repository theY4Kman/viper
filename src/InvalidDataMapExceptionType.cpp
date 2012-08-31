#include "InvalidDataMapExceptionType.h"
#include "Util.h"

InvalidDataMapExceptionType::InvalidDataMapExceptionType(int entityIndex) {
	EntityIndex = entityIndex;
}

std::string InvalidDataMapExceptionType::what() const {
	char str[256];
	UTIL_Format(str, sizeof(str), "Entity does not have a valid datamap: #%d", EntityIndex);

	return std::string(str);
}