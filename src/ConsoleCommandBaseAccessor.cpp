#include "BoostPythonSM.h"
#include "ConsoleCommandBaseAccessor.h"
#include "sdk/smsdk_ext.h"
#include "Macros.h"
#include "ViperExtension.h"
#include "CompatWrappers.h"

namespace py = boost::python;

ConsoleCommandBaseAccessor console__CommandBaseAccessor;

bool ConsoleCommandBaseAccessor::RegisterConCommandBase(ConCommandBase *conCommandBase) {
	META_REGCVAR(conCommandBase);
	return true;
}