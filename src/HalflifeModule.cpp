#include "BoostPythonSM.h"
#include "HalflifeModule.h"
#include "sdk/smsdk_ext.h"
#include "Macros.h"

namespace py = boost::python;

void halflife__log_to_game(std::string text, bool appendNewline = true) {
	if(appendNewline) {
		text.append("\n");
	}

	engine->LogPrint(text.c_str());
}

std::string halflife__get_game_description(bool original = false) {
	if(original) {
		return std::string(gamedll->GetGameDescription());
	}

	return std::string(SERVER_CALL(GetGameDescription)());
}

void halflife__set_random_seed(int seed) {
	g_Interfaces.UniformRandomStreamInstance->SetSeed(seed);
}

float halflife__get_random_float(float minimum = 0.0f, float maximum = 1.0f) {
	return g_Interfaces.UniformRandomStreamInstance->RandomFloat(minimum, maximum);
}

int halflife__get_random_int(int minimum, int maximum) {
	return g_Interfaces.UniformRandomStreamInstance->RandomInt(minimum, maximum);
}

bool halflife__is_map_valid(std::string map) {
	return engine->IsMapValid(map.c_str());
}

bool halflife__is_dedicated_server() {
	return engine->IsDedicatedServer();
}

float halflife__get_engine_time() {
	#if SOURCE_ENGINE >= SE_LEFT4DEAD2
	float time = Plat_FloatTime();
#else
	float time = engine->Time();
#endif
	
	return time;
}

float halflife__get_game_time() {
	return g_Interfaces.GlobalVarsInstance->curtime;
}

int halflife__get_game_tick_count() {
	return g_Interfaces.GlobalVarsInstance->tickcount;
}

std::string halflife__get_game_folder_name() {
	return std::string(g_pSM->GetGameFolderName());
}

std::string halflife__get_current_map() {
	return std::string(gamehelpers->GetCurrentMap());
}

int halflife__precache_model(std::string model, bool preload = false) {
	return engine->PrecacheModel(model.c_str(), preload);
}

int halflife__precache_sentence_file(std::string file, bool preload = false) {
	return engine->PrecacheSentenceFile(file.c_str(), preload);
}

int halflife__precache_decal(std::string decal, bool preload = false) {
	return engine->PrecacheDecal(decal.c_str(), preload);
}

int halflife__precache_generic(std::string genericFile, bool preload = false) {
	return engine->PrecacheGeneric(genericFile.c_str(), preload);
}

bool halflife__precache_sound(std::string sound, bool preload = false) {
	return g_Interfaces.EngineSoundInstance->PrecacheSound(sound.c_str(), preload);
}

bool halflife__is_model_precached(std::string model) {
	return engine->IsModelPrecached(model.c_str());
}

bool halflife__is_generic_precached(std::string genericFile) {
	return engine->IsGenericPrecached(genericFile.c_str());
}

bool halflife__is_decal_precached(std::string decal) {
	return engine->IsDecalPrecached(decal.c_str());
}

bool halflife__is_sound_precached(std::string sound) {
	return g_Interfaces.EngineSoundInstance->IsSoundPrecached(sound.c_str());
}

BOOST_PYTHON_MODULE(Halflife) {
	py::def("LogToGame", &halflife__log_to_game, (py::arg("text"), py::arg("append_newline") = true));
	py::def("SetRandomSeed", &halflife__set_random_seed, (py::arg("seed")));
	py::def("GetRandomFloat", &halflife__get_random_float, (py::arg("minimum") = 0.0f, py::arg("maximum") = 1.0f));
	py::def("GetRandomInt", &halflife__get_random_int, (py::arg("minimum"), py::arg("maximum")));
	py::def("IsMapValid", &halflife__is_map_valid, (py::arg("map")));
	py::def("IsDedicatedServer", &halflife__is_dedicated_server);
	py::def("GetEngineTime", &halflife__get_engine_time);
	py::def("GetGameTime", &halflife__get_game_time);
	py::def("GetGameTickCount", &halflife__get_game_tick_count);
	py::def("GetGameDescription", &halflife__get_game_description, (py::arg("original") = false));
	py::def("GetGameFolderName", &halflife__get_game_folder_name);
	py::def("GetCurrentMap", &halflife__get_current_map);
	py::def("PrecacheModel", &halflife__precache_model, (py::arg("model"), py::arg("preload") = false));
	py::def("PrecacheSentenceFile", &halflife__precache_sentence_file, (py::arg("file"), py::arg("preload") = false));
	py::def("PrecacheDecal", &halflife__precache_decal, (py::arg("decal"), py::arg("preload") = false));
	py::def("PrecacheGeneric", &halflife__precache_generic, (py::arg("generic"), py::arg("preload") = false));
	py::def("PrecacheSound", &halflife__precache_sound, (py::arg("sound"), py::arg("preload") = false));
	py::def("IsModelPrecached", &halflife__is_model_precached, (py::arg("model")));
	py::def("IsGenericPrecached", &halflife__is_generic_precached, (py::arg("generic")));
	py::def("IsDecalPrecached", &halflife__is_decal_precached, (py::arg("decal")));
	py::def("IsSoundPrecached", &halflife__is_sound_precached, (py::arg("sound")));
}

void destroyHalflife() {
}