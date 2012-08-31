#include "BoostPythonSM.h"
#include "SourcemodModule.h"
#include "sdk/smsdk_ext.h"
#include "LoadExtensionException.h"
#include "Macros.h"

namespace py = boost::python;

void sourcemod_require_ext(std::string extensionName) {
	std::string fullExtensionName = extensionName + ".ext";

	char error[256];
	if (extsys->LoadExtension(fullExtensionName.c_str(),
		(char *)&error, sizeof(error))) {
		return;
	}
	
	throw LoadExtensionException(fullExtensionName, error);
}

std::string sourcemod_get_game_path() {
	return g_pSM->GetGamePath();
}

std::string sourcemod_get_sourcemod_path(py::tuple argumentsTuple,
	py::dict keywordsDict) {
	return std::string("");
}

DEFINE_CUSTOM_EXCEPTION_INIT(LoadExtensionException, sourcemod)

BOOST_PYTHON_MODULE(sourcemod) {
	py::def("require_ext", sourcemod_require_ext);
	py::def("get_sourcemod_path", py::raw_function(sourcemod_get_sourcemod_path,
		0));
	py::def("get_game_path", sourcemod_get_game_path);
	
	py::enum_<SourceMod::ResultType>("ResultType")
		.value("Continue", SourceMod::Pl_Continue)
		.value("Changed", SourceMod::Pl_Changed)
		.value("Handled", SourceMod::Pl_Handled)
		.value("Stop", SourceMod::Pl_Stop);

	DEFINE_CUSTOM_EXCEPTION(LoadExtensionException, sourcemod,
		PyExc_Exception, "sourcemod.LoadExtensionException",
		"LoadExtensionException")
}