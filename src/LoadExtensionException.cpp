#include "LoadExtensionException.h"

LoadExtensionException::LoadExtensionException(
	std::string fullExtensionName, std::string error) {
	FullExtensionName = fullExtensionName;
	Error = error;
}

std::string LoadExtensionException::what() const {
	return "Unable to load extension " + FullExtensionName + ": " +
		Error;
}