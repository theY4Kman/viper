#include "BoostPythonSM.h"
#include "UserMessageType.h"
#include "Macros.h"
#include "CompatWrappers.h"
#include "ViperExtension.h"

namespace py = boost::python;

UserMessageType::UserMessageType(int id, std::string name) {
	ID = id;
	Name = name;
}