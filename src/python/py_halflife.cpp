/**
 * =============================================================================
 * Viper
 * Copyright (C) 2007-2010 Zach "theY4Kman" Kanzler
 * Copyright (C) 2004-2007 AlliedModders LLC.
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <Python.h>
#include "extension.h"
#include "HalfLife2.h"
#include "viper_metamod_wrappers.h"

static PyObject *
halflife__get_engine_time(PyObject *self)
{
#if SOURCE_ENGINE == SE_LEFT4DEAD2
    float fTime = Plat_FloatTime();
#else
    float fTime = engine->Time();
#endif
    
    return PyFloat_FromDouble(fTime);
}

static PyObject *
halflife__get_game_description(PyObject *self, PyObject *args, PyObject *kwds)
{
    bool original = false;
    static char *keywdlist[] = {"original", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|b", keywdlist, &original))
        return NULL;
    
    char const *description;
    if (original)
        description = gamedll->GetGameDescription();
    else
        description = SERVER_CALL(GetGameDescription)();
    
    return PyString_FromString(description);
}

static PyObject *
halflife__get_game_folder_name(PyObject *self)
{
    return PyString_FromString(g_pSM->GetGameFolderName());
}

static PyObject *
halflife__get_game_time(PyObject *self)
{
    return PyFloat_FromDouble(g_SMAPI->GetCGlobals()->curtime);
}

static PyObject *
halflife__get_current_map(PyObject *self)
{
    return PyString_FromString(gamehelpers->GetCurrentMap());
}

static PyObject *
halflife__guess_sdk_version(PyObject *self)
{
#if defined METAMOD_PLAPI_VERSION
    int version = g_SMAPI->GetSourceEngineBuild();

    switch (version)
    {
    case SOURCE_ENGINE_ORIGINAL:
        return PyInt_FromLong(10);
    
    case SOURCE_ENGINE_DARKMESSIAH:
        return PyInt_FromLong(15);
    
    case SOURCE_ENGINE_EPISODEONE:
        return PyInt_FromLong(20);
    
    case SOURCE_ENGINE_ORANGEBOX:
        return PyInt_FromLong(30);
    }
#else
    /* The Ship is the only known game to use the old engine */
    if (strcasecmp(g_pSM->GetGameFolderName(), "ship") == 0)
        return PyInt_FromLong(10);
    else
        return PyInt_FromLong(20);
#endif

    return PyInt_FromLong(0);
}

static PyObject *
halflife__is_dedicated_server(PyObject *self)
{
    PyObject *ret = engine->IsDedicatedServer() ? Py_True : Py_False;
    Py_INCREF(ret);
    
    return ret;
}

static PyObject *
halflife__is_decal_precached(PyObject *self, PyObject *args)
{
    char const *decal;
    if (!PyArg_ParseTuple(args, "s", &decal))
        return NULL;
    
    return PyBool_FromLong(engine->IsDecalPrecached(decal));
}

static PyObject *
halflife__is_generic_precached(PyObject *self, PyObject *args)
{
    char const *generic;
    if (!PyArg_ParseTuple(args, "s", &generic))
        return NULL;
    
    return PyBool_FromLong(engine->IsGenericPrecached(generic));
}

static PyObject *
halflife__is_map_valid(PyObject *self, PyObject *args)
{
    char *map;
    if (!PyArg_ParseTuple(args, "s", &map))
        return NULL;
    
    PyObject *ret = engine->IsMapValid(map) ? Py_True : Py_False;
    Py_INCREF(ret);
    
    return ret;
}

static PyObject *
halflife__is_model_precached(PyObject *self, PyObject *args)
{
    char const *model;
    if (!PyArg_ParseTuple(args, "s", &model))
        return NULL;
    
    return PyBool_FromLong(engine->IsModelPrecached(model));
}

static PyObject *
halflife__is_sound_precached(PyObject *self, PyObject *args)
{
    char const *sound;
    if (!PyArg_ParseTuple(args, "s", &sound))
        return NULL;
    
    return PyBool_FromLong(enginesound->IsSoundPrecached(sound));
}

static PyObject *
halflife__precache_decal(PyObject *self, PyObject *args, PyObject *kwds)
{
    char const *decal;
    bool preload = false;
    static char *keywdlist[] = {"decal", "preload"};
    
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s|b", keywdlist, &decal, &preload))
        return NULL;
    
    return PyInt_FromLong(engine->PrecacheDecal(decal, preload));
}

static PyObject *
halflife__precache_generic(PyObject *self, PyObject *args, PyObject *kwds)
{
    char const *generic;
    bool preload = false;
    static char *keywdlist[] = {"generic", "preload"};
    
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s|b", keywdlist, &generic, &preload))
        return NULL;
    
    return PyInt_FromLong(engine->PrecacheGeneric(generic, preload));
}

static PyObject *
halflife__precache_model(PyObject *self, PyObject *args, PyObject *kwds)
{
    char const *model;
    bool preload = false;
    static char *keywdlist[] = {"model", "preload"};
    
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s|b", keywdlist, &model, &preload))
        return NULL;
    
    return PyInt_FromLong(engine->PrecacheModel(model, preload));
}

static PyObject *
halflife__precache_sentence_file(PyObject *self, PyObject *args, PyObject *kwds)
{
    char const *sentence_file;
    bool preload = false;
    static char *keywdlist[] = {"sentence_file", "preload"};
    
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s|b", keywdlist, &sentence_file, &preload))
        return NULL;
    
    return PyInt_FromLong(engine->PrecacheSentenceFile(sentence_file, preload));
}

static PyObject *
halflife__precache_sound(PyObject *self, PyObject *args, PyObject *kwds)
{
    char const *sound;
    bool preload = false;
    static char *keywdlist[] = {"sound", "preload"};
    
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s|b", keywdlist, &sound, &preload))
        return NULL;
    
    return PyBool_FromLong(enginesound->PrecacheSound(sound, preload));
}

static PyMethodDef halflife__methods[] = {
#if NOT_DOCUMENTED_YET
    {"create_dialog"},
#endif
    {"get_current_map", (PyCFunction)halflife__get_current_map, METH_NOARGS,
        "get_current_map() -> str\n\n"
        "Returns the current map name\n\n"
        "@rtype: str\n"
        "@return The map name, excluding the .bsp extension"},
    {"get_engine_time", (PyCFunction)halflife__get_engine_time, METH_NOARGS,
        "get_engine_time() -> float\n\n"
        "Returns a high-precision time value for profiling the engine.\n\n"
        "@rtype: float\n"
        "@return: A floating point time value"},
    {"get_game_description", (PyCFunction)halflife__get_game_description, METH_VARARGS|METH_KEYWORDS,
        "get_game_description([original=False]) -> str\n\n"
        "Returns the game description of the mod.\n\n"
        "@type  original: bool\n"
        "@param original: If True, retrieves the original game description, ignoring any\n"
        "    potential hooks from plug-ins.\n"
        "@rtype: str\n"
        "@return: The description of the game."},
    {"get_game_folder_name", (PyCFunction)halflife__get_game_folder_name, METH_NOARGS,
        "get_game_folder_name() -> str\n\n"
        "Returns the name of the game's directory (e.g. 'cstrike', 'tf')\n\n"
        "@rtype: str\n"
        "@return: The directory name."},
    {"get_game_time", (PyCFunction)halflife__get_game_time, METH_NOARGS,
        "get_game_time() -> float\n\n"
        "Returns the game time based on the game tick.\n\n"
        "@rtype: float\n"
        "@return: Game tick time"},
    {"guess_sdk_version", (PyCFunction)halflife__guess_sdk_version, METH_NOARGS,
        "guess_sdk_version() -> int\n\n"
        "Guesses the SDK version a mod was compiled against. If nothing specific is\n"
        "known about the game, the engine version is used instead. The return values\n"
        "are guaranteed to increase chronologically (that is, a later release will have\n"
        "a higher value).\n\n"
        "@rtype: SOURCE_SDK constant\n"
        "@return: A sourcemod.halflife.SOURCE_SDK_* constant."},
    {"is_decal_precached", halflife__is_decal_precached, METH_VARARGS,
        "is_decal_precached(decal) -> bool\n\n"
        "Returns if a given decal is precached.\n\n"
        "@type  decal: str\n"
        "@param decal: Name of the decal to check\n"
        "@rtype: bool\n"
        "@return: True if precached, False otherwise."},
    {"is_dedicated_server", (PyCFunction)halflife__is_dedicated_server, METH_NOARGS,
        "is_dedicated_server() -> bool\n\n"
        "Returns whether the server is dedicated.\n\n"
        "@rtype: bool\n"
        "@return: True if dedicated, False otherwise."},
    {"is_generic_precached", halflife__is_generic_precached, METH_VARARGS,
        "is_generic_precached(generic) -> bool\n\n"
        "Returns if a given generic file is precached.\n\n"
        "@type  generic: str\n"
        "@param generic: Name of the generic to check\n"
        "@rtype: bool\n"
        "@return: True if precached, False otherwise."},
    {"is_map_valid", halflife__is_map_valid, METH_VARARGS,
        "is_map_valid(map) -> bool\n\n"
        "Returns whether a map is valid or not.\n\n"
        "@type  map: str\n"
        "@param map: Map name, excluding .bsp extension\n"
        "@rtype: bool\n"
        "@return: True if valid, False otherwise."},
    {"is_model_precached", halflife__is_model_precached, METH_VARARGS,
        "is_model_precached(model) -> bool\n\n"
        "Returns if a given model is precached.\n\n"
        "@type  model: str\n"
        "@param model: Name of the model to check\n"
        "@rtype: bool\n"
        "@return: True if precached, False otherwise."},
    {"is_sound_precached", halflife__is_sound_precached, METH_VARARGS,
        "is_sound_precached(sound) -> bool\n\n"
        "Returns if a given sound is precached.\n\n"
        "@type  sound: str\n"
        "@param sound: Name of the sound to check\n"
        "@rtype: bool\n"
        "@return: True if precached, False otherwise."},
    {"precache_decal", (PyCFunction)halflife__precache_decal, METH_VARARGS|METH_KEYWORDS,
        "precache_decal(decal[, preload=False]) -> int\n\n"
        "Precaches a given decal.\n\n"
        "@type  decal: str\n"
        "@param decal: The name of the decal to precache.\n"
        "@type  preload: bool\n"
        "@param preload: If True, the file will be precached before level startup.\n"
        "@rtype: int\n"
        "@return: A valid decal index on success, 0 otherwise."},
    {"precache_generic", (PyCFunction)halflife__precache_generic, METH_VARARGS|METH_KEYWORDS,
        "precache_generic(generic[, preload=False]) -> int\n\n"
        "Precaches a given generic file.\n\n"
        "@type  generic: str\n"
        "@param generic: The name of the generic file to precache.\n"
        "@type  preload: bool\n"
        "@param preload: If True, the file will be precached before level startup.\n"
        "@rtype: int\n"
        "@return: A valid generic file index on success, 0 otherwise."},
    {"precache_model", (PyCFunction)halflife__precache_model, METH_VARARGS|METH_KEYWORDS,
        "precache_model(model[, preload=False]) -> int\n\n"
        "Precaches a given model.\n\n"
        "@type  model: str\n"
        "@param model: The name of the model to precache.\n"
        "@type  preload: bool\n"
        "@param preload: If True, the file will be precached before level startup.\n"
        "@rtype: int\n"
        "@return: A valid model index on success, 0 otherwise."},
    {"precache_sentence_file", (PyCFunction)halflife__precache_sentence_file, METH_VARARGS|METH_KEYWORDS,
        "precache_sentence_file(sentence_file[, preload=False]) -> int\n\n"
        "Precaches a given sentence file.\n\n"
        "@type  sentence_file: str\n"
        "@param sentence_file: The name of the sentence file to precache.\n"
        "@type  preload: bool\n"
        "@param preload: If True, the file will be precached before level startup.\n"
        "@rtype: int\n"
        "@return: A valid sentence file index on success, 0 otherwise."},
    {"precache_sound", (PyCFunction)halflife__precache_sound, METH_VARARGS|METH_KEYWORDS,
        "precache_sound(sound[, preload=False]) -> bool\n\n"
        "Precaches a given sound.\n\n"
        "@type  sound: str\n"
        "@param sound: The name of the sound to precache.\n"
        "@type  preload: bool\n"
        "@param preload: If True, the file will be precached before level startup.\n"
        "@rtype: bool\n"
        "@return: True on success, False otherwise."},
    {NULL, NULL, 0, NULL}
};

PyObject *
inithalflife(void)
{
    PyObject *halflife = Py_InitModule3("halflife", halflife__methods,
        "Generic Source engine functions and objects.");
    
    PyModule_AddIntConstant(halflife, "SOURCE_SDK_UNKNOWN", 0);
    PyModule_AddIntConstant(halflife, "SOURCE_SDK_ORIGINAL", 10);
    PyModule_AddIntConstant(halflife, "SOURCE_SDK_DARKMESSIAH", 15);
    PyModule_AddIntConstant(halflife, "SOURCE_SDK_EPISODE1", 20);
    PyModule_AddIntConstant(halflife, "SOURCE_SDK_EPISODE2", 30);
    PyModule_AddIntConstant(halflife, "SOURCE_SDK_LEFT4DEAD", 40);
    
    return halflife;
}
