#include "BoostPythonSM.h"
#include "SysHooks.h"
#include "sdk/smsdk_ext.h"
#include "Macros.h"
#include "NullReferenceExceptionType.h"
#include "StdIoRedirect.h"

namespace py = boost::python;

namespace Viper {
	namespace Modules {
		namespace Sys {
			void sys__NullReferenceExceptionTypeTranslator(const NullReferenceExceptionType &e) {
				PyErr_Format(PyExc_MemoryError, e.what().c_str());
			}

			void initsyshooks() {
				py::object mainModule = py::import("__main__");
				py::object mainNamespace = mainModule.attr("__dict__");

				// Hooking Stdout/err
				mainNamespace["StdIoRedirect"] = py::class_
					<StdIoRedirect>("StdIoRedirect", py::init<>())
					.def("write", &StdIoRedirect::Write)
					.def("Flush", &StdIoRedirect::Flush);

				StdIoRedirect stdIoRedirector;

				py::object sysModule = py::import("sys");

				sysModule.attr("stderr") = stdIoRedirector;
				sysModule.attr("stdout") = stdIoRedirector;

				py::register_exception_translator<NullReferenceExceptionType>(
					sys__NullReferenceExceptionTypeTranslator);
			}
		}
	}
}