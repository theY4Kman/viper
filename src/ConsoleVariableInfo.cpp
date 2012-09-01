#include "BoostPythonSM.h"
#include "ConsoleVariableInfo.h"
#include "sdk/smsdk_ext.h"
#include "Macros.h"
#include "ViperExtension.h"
#include "CompatWrappers.h"

namespace py = boost::python;

ConsoleVariableInfo::ConsoleVariableInfo(std::string name, ConVar *conVarInstance, bool isFromViper) {
	Name = name;
	ConVarInstance = conVarInstance;
	IsFromViper = isFromViper;
}