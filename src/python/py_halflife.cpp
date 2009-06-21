/**
 * =============================================================================
 * Viper
 * Copyright (C) 2008-2009 Zach "theY4Kman" Kanzler
 * Copyright (C) 2004-2007 AlliedModders LLC.  All rights reserved.
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
    return engine->IsDedicatedServer() ? Py_True : Py_False;
}

static PyObject *
halflife__is_map_valid(PyObject *self, PyObject *args)
{
    char *map;
    if (!PyArg_ParseTuple(args, "s", &map))
        return NULL;
    
    return engine->IsMapValid(map) ? Py_True : Py_False;
}

static PyMethodDef halflife__methods[] = {
#if NOT_DOCUMENTED_YET
    {"create_dialog"},
    {"is_decal_precached"},
    {"is_generic_precached"},
    {"is_model_precached"},
    {"is_sound_precached"},
    {"precache_decal"},
    {"precache_generic"},
    {"precache_model"},
    {"precache_sentence_file"},
    {"show_vgui_panel"},
#endif
#if NOT_IMPLEMENTED_YET
    {"get_engine_time", (PyCFunction)halflife__get_engine_time, METH_NOARGS,
        "get_engine_time() -> float\n\n"
        "Returns a high-precision time value for profiling the engine.\n"
        "@rtype: float\n"
        "@return: A floating point time value"},
    {"get_game_description", halflife__get_game_description, METH_VARARGS,
        "get_game_description(original=False) -> str\n\n"
        "Returns the game description of the mod.\n"
        "@type  original: bool\n"
        "@param original: If True, retrieves the original game description, ignoring any\n"
        "    potential hooks from plug-ins.\n"
        "@rtype: str\n"
        "@return: The description of the game."},
    {"get_game_folder_name", (PyCFunction)halflife__get_game_folder_name, METH_NOARGS,
        "get_game_folder_name() -> str\n\n"
        "Returns the name of the game's directory (e.g. 'cstrike', 'tf')\n"
        "@rtype: str\n"
        "@return: The directory name."},
    {"get_game_time", (PyCFunction)halflife__get_game_time, METH_NOARGS,
        "get_game_time() -> float\n\n"
        "Returns the game time based on the game tick.\n"
        "@rtype: float\n"
        "@return: Game tick time"},
#endif
    {"get_current_map", (PyCFunction)halflife__get_current_map, METH_NOARGS,
        "get_current_map() -> str\n\n"
        "Returns the current map name\n"
        "@rtype: str\n"
        "@return The map name, excluding the .bsp extension"},
    {"guess_sdk_version", (PyCFunction)halflife__guess_sdk_version, METH_NOARGS,
        "guess_sdk_version() -> int\n\n"
        "Guesses the SDK version a mod was compiled against. If nothing specific is\n"
        "known about the game, the engine version is used instead. The return values\n"
        "are guaranteed to increase chronologically (that is, a later release will have\n"
        "a higher value).\n"
        "@rtype: SOURCE_SDK constant\n"
        "@return: A sourcemod.halflife.SOURCE_SDK_* constant."},
    {"is_dedicated_server", (PyCFunction)halflife__is_dedicated_server, METH_NOARGS,
        "is_dedicated_server() -> bool\n\n"
        "Returns whether the server is dedicated.\n"
        "@rtype: bool\n"
        "@return: True if dedicated, False otherwise."},
    {"is_map_valid", halflife__is_map_valid, METH_VARARGS,
        "is_map_valid(map) -> bool\n\n"
        "Returns whether a map is valid or not.\n"
        "@type  map: str\n"
        "@param map: Map name, excluding .bsp extension\n"
        "@rtype: bool\n"
        "@return: True if valid, False otherwise."},
    {NULL, NULL, 0, NULL}
};

PyObject *
inithalflife(void)
{
    PyObject *halflife = Py_InitModule3("halflife", NULL,
        "Generic Source engine functions and objects.");
    
    PyModule_AddIntConstant(halflife, "SOURCE_SDK_UNKNOWN", 0);
    PyModule_AddIntConstant(halflife, "SOURCE_SDK_ORIGINAL", 10);
    PyModule_AddIntConstant(halflife, "SOURCE_SDK_DARKMESSIAH", 15);
    PyModule_AddIntConstant(halflife, "SOURCE_SDK_EPISODE1", 20);
    PyModule_AddIntConstant(halflife, "SOURCE_SDK_EPISODE2", 30);
    PyModule_AddIntConstant(halflife, "SOURCE_SDK_LEFT4DEAD", 40);
    
    return halflife;
}
