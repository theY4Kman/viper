================================================
:mod:`sourcemod.halflife` --- Engine Interaction
================================================

..  module:: sourcemod.halflife
..  moduleauthor:: Zach "theY4Kman" Kanzler <they4kman@gmail.com>

The :mod:`halflife` module contains functions that interact with the HL2 engine.

Module Contents
===============

..  _SOURCE_SDK-constants:

..  data::
    SOURCE_SDK_UNKNOWN
    SOURCE_SDK_ORIGINAL
    SOURCE_SDK_DARKMESSIAH
    SOURCE_SDK_EPISODE1
    SOURCE_SDK_EPISODE2
    SOURCE_SDK_EPISODE2VALVE
    SOURCE_SDK_LEFT4DEAD
    SOURCE_SDK_LEFT4DEAD2
    SOURCE_SDK_ALIENSWARM
    
    Constants that identify Source Engine versions.

..  function:: get_current_map()
    
    Returns the current map name, excluding the ".bsp" extension

..  function:: get_engine_time()
    
    Returns a float containing a high-precision time value for profiling the engine.
    
    :rtype: float

..  function:: get_game_description([original=False])
    
    Returns the game description of the mod. If ``original`` is True, it retrieves the original game description, ignoring any potential hooks from plug-ins.

..  function:: get_game_folder_name()
    
    Returns the name of game's directory, e.g. 'cstrike' or 'tf'

..  function:: get_game_time()
    
    Returns the game time based on the game tick as a float.
    
    :rtype: float

..  function:: guess_sdk_version()
    
    Guesses the SDK version a mod was compiled against. If nothing specific is known about the game, the engine version is used instead. The return values are guaranteed to increase chronologically (that is, a later release will have a higher value).
    
    Returns a :ref:`SOURCE_SDK constant <SOURCE_SDK-constants>` identifying the engine version.
    
..  function:: is_decal_precached(decal)
    
    Returns whether or not the given decal is precached.

..  function:: is_dedicated_server()
    
    Returns whether the server is dedicated.

..  function:: is_generic_precached(generic)
    
    Returns whether the given generic file is precached.

..  function:: is_map_valid(map)
    
    Returns whether the given map is valid or not. ``map`` should contain a map name, excluding the ".bsp" extension.

..  function:: is_model_precached(model)
    
    Returns whether the given model is precached.

..  function:: is_sound_precached(sound)
    
    Returns whether the sound is precached.

..  function:: precache_decal(decal[, preload=False])
               precache_generic(generic[, preload=False])
               precache_model(model[, preload=False])
               precache_sentence_file(sentence_file[, preload=False])
               precache_sound(sound[, preload=False])
    
    Precaches the passed decal, generic file, etc. If ``preload`` is True, the file will be precached before level startup.
