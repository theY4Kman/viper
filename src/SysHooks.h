#ifndef __INCLUDE_SYSHOOKS_H__
#define __INCLUDE_SYSHOOKS_H__

#include "Macros.h"
#include "BoostPythonSM.h"

extern "C" __declspec(dllexport) void initsyshooks(boost::python::object mainNamespace, boost::python::object sysModule);

#endif