#include "BoostPythonSM.h"
#include "EventFieldType.h"
#include "Macros.h"
#include "CompatWrappers.h"
#include "ViperExtension.h"

namespace py = boost::python;

EventFieldType::EventFieldType(std::string name, EventFieldTypes fieldType) {
	Name = name;
	FieldType = fieldType;
}