#include "BoostPythonSM.h"
#include "SysHooks.h"
#include "sdk/smsdk_ext.h"
#include "Macros.h"
#include "NullReferenceExceptionType.h"
#include "StdIoRedirect.h"

namespace py = boost::python;

void sys__NullReferenceExceptionTypeTranslator(const NullReferenceExceptionType &e) {
	PyErr_Format(PyExc_MemoryError, e.what().c_str());
}

void initsyshooks(py::object mainNamespace, py::object sysModule) {
	// Hooking Stdout/err
	static py::object redirectType = py::class_
		<StdIoRedirect>("StdIoRedirect", py::init<>())
		.def("write", &StdIoRedirect::Write)
		.def("Flush", &StdIoRedirect::Flush);

	mainNamespace["StdIoRedirect"] = redirectType;

	StdIoRedirect stdIoRedirector;

	sysModule.attr("stderr") = stdIoRedirector;
	sysModule.attr("stdout") = stdIoRedirector;

	py::register_exception_translator<NullReferenceExceptionType>(
		sys__NullReferenceExceptionTypeTranslator);
}