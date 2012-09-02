#include "BoostPythonSM.h"
#include "SourcemodModule.h"
#include "sdk/smsdk_ext.h"
#include "LoadExtensionException.h"
#include "Macros.h"

namespace py = boost::python;

void sourcemod__require_ext(std::string extensionName) {
	std::string fullExtensionName = extensionName + ".ext";

	char error[256];
	if (extsys->LoadExtension(fullExtensionName.c_str(),
		(char *)&error, sizeof(error))) {
		return;
	}
	
	throw LoadExtensionException(fullExtensionName, error);
}

std::string sourcemod__get_game_path(std::string path = std::string()) {
	std::string fullPath(g_pSM->GetGamePath());

	fullPath.append(path.c_str());

	return fullPath;
}

std::string sourcemod__get_sourcemod_path(std::string path = std::string()) {
	std::string fullPath(g_pSM->GetSourceModPath());

	fullPath.append(path.c_str());

	return fullPath;
}

DEFINE_CUSTOM_EXCEPTION_INIT(LoadExtensionException, Sourcemod)

BOOST_PYTHON_MODULE(Sourcemod) {
	py::def("RequireExt", sourcemod__require_ext);
	py::def("GetSourcemodPath", sourcemod__get_sourcemod_path, (py::arg("path") = std::string()));
	py::def("GetGamePath", sourcemod__get_game_path, (py::arg("path") = std::string()));
	
	py::enum_<SourceMod::ResultType>("ResultType")
		.value("Continue", SourceMod::Pl_Continue)
		.value("Changed", SourceMod::Pl_Changed)
		.value("Handled", SourceMod::Pl_Handled)
		.value("Stop", SourceMod::Pl_Stop);

	DEFINE_CUSTOM_EXCEPTION(LoadExtensionException, Sourcemod,
		PyExc_Exception, "Sourcemod.LoadExtensionException",
		"LoadExtensionException")
}

void destroySourcemod() {
}