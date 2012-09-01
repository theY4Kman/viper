#include "BoostPythonSM.h"
#include "ConsoleCommandInfo.h"
#include "sdk/smsdk_ext.h"
#include "Macros.h"
#include "ViperExtension.h"
#include "CompatWrappers.h"

namespace py = boost::python;

ConsoleCommandInfo::ConsoleCommandInfo(std::string name, ConCommand *conCommandInstance, bool isFromViper) {
	Name = name;
	ConCommandInstance = conCommandInstance;
	IsFromViper = isFromViper;
}