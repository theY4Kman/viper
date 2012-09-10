#include "ConsoleCommandBaseAccessor.h"
#include "STL.h"
#include "sdk/smsdk_ext.h"
#include "CompatWrappers.h"

ConsoleCommandBaseAccessor console__CommandBaseAccessor;

bool ConsoleCommandBaseAccessor::RegisterConCommandBase(ConCommandBase *conCommandBase) {
	META_REGCVAR(conCommandBase);
	return true;
}