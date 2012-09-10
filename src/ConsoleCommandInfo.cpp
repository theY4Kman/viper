#include "ConsoleCommandInfo.h"

ConsoleCommandInfo::ConsoleCommandInfo(std::string name, ConCommand *conCommandInstance, bool isFromViper) {
	Name = name;
	ConCommandInstance = conCommandInstance;
	IsFromViper = isFromViper;
}