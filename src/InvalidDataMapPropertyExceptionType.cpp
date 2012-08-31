#include "InvalidDataMapPropertyExceptionType.h"
#include "Util.h"

InvalidDataMapPropertyExceptionType::InvalidDataMapPropertyExceptionType(int entityIndex, std::string propertyName) {
	EntityIndex = entityIndex;
	PropertyName = propertyName;
}

std::string InvalidDataMapPropertyExceptionType::what() const {
	char str[256];
	UTIL_Format(str, sizeof(str), "Entity #%d data map property invalid: %s", EntityIndex, PropertyName.c_str());

	return std::string(str);
}