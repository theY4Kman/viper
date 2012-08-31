#include "EntityNotNetworkableExceptionType.h"
#include "Util.h"

EntityNotNetworkableExceptionType::EntityNotNetworkableExceptionType(int entityIndex) {
	EntityIndex = entityIndex;
}

std::string EntityNotNetworkableExceptionType::what() const {
	char str[256];
	UTIL_Format(str, sizeof(str), "Not a networkable entity: #%d", EntityIndex);

	return std::string(str);
}