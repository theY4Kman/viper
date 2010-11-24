SOURCE_SDK_DARKMESSIAH = 15
SOURCE_SDK_EPISODE1 = 20
SOURCE_SDK_EPISODE2 = 30
SOURCE_SDK_LEFT4DEAD = 40
SOURCE_SDK_ORIGINAL = 10
SOURCE_SDK_UNKNOWN = 0

def get_current_map():
  '''get_current_map() -> str
  
  Returns the current map name
  
  @rtype: str
  @return The map name, excluding the .bsp extension'''
  pass

def get_engine_time():
  '''get_engine_time() -> float
  
  Returns a high-precision time value for profiling the engine.
  
  @rtype: float
  @return: A floating point time value'''
  pass

def get_game_description(original=False):
  '''get_game_description([original=False]) -> str
  
  Returns the game description of the mod.
  
  @type  original: bool
  @param original: If True, retrieves the original game description, ignoring any
      potential hooks from plug-ins.
  @rtype: str
  @return: The description of the game.'''
  pass

def get_game_folder_name():
  '''get_game_folder_name() -> str
  
  Returns the name of the game's directory (e.g. 'cstrike', 'tf')
  
  @rtype: str
  @return: The directory name.'''
  pass

def get_game_time():
  '''get_game_time() -> float
  
  Returns the game time based on the game tick.
  
  @rtype: float
  @return: Game tick time'''
  pass

def guess_sdk_version():
  '''guess_sdk_version() -> int
  
  Guesses the SDK version a mod was compiled against. If nothing specific is
  known about the game, the engine version is used instead. The return values
  are guaranteed to increase chronologically (that is, a later release will have
  a higher value).
  
  @rtype: SOURCE_SDK constant
  @return: A sourcemod.halflife.SOURCE_SDK_* constant.'''
  pass

def is_decal_precached(decal):
  '''is_decal_precached(decal) -> bool
  
  Returns if a given decal is precached.
  
  @type  decal: str
  @param decal: Name of the decal to check
  @rtype: bool
  @return: True if precached, False otherwise.'''
  pass

def is_dedicated_server():
  '''is_dedicated_server() -> bool
  
  Returns whether the server is dedicated.
  
  @rtype: bool
  @return: True if dedicated, False otherwise.'''
  pass

def is_generic_precached(generic):
  '''is_generic_precached(generic) -> bool
  
  Returns if a given generic file is precached.
  
  @type  generic: str
  @param generic: Name of the generic to check
  @rtype: bool
  @return: True if precached, False otherwise.'''
  pass

def is_map_valid(map):
  '''is_map_valid(map) -> bool
  
  Returns whether a map is valid or not.
  
  @type  map: str
  @param map: Map name, excluding .bsp extension
  @rtype: bool
  @return: True if valid, False otherwise.'''
  pass

def is_model_precached(model):
  '''is_model_precached(model) -> bool
  
  Returns if a given model is precached.
  
  @type  model: str
  @param model: Name of the model to check
  @rtype: bool
  @return: True if precached, False otherwise.'''
  pass

def is_sound_precached(sound):
  '''is_sound_precached(sound) -> bool
  
  Returns if a given sound is precached.
  
  @type  sound: str
  @param sound: Name of the sound to check
  @rtype: bool
  @return: True if precached, False otherwise.'''
  pass

def precache_decal(decal, preload=False):
  '''precache_decal(decal[, preload=False]) -> int
  
  Precaches a given decal.
  
  @type  decal: str
  @param decal: The name of the decal to precache.
  @type  preload: bool
  @param preload: If True, the file will be precached before level startup.
  @rtype: int
  @return: A valid decal index on success, 0 otherwise.'''
  pass

def precache_generic(generic, preload=False):
  '''precache_generic(generic[, preload=False]) -> int
  
  Precaches a given generic file.
  
  @type  generic: str
  @param generic: The name of the generic file to precache.
  @type  preload: bool
  @param preload: If True, the file will be precached before level startup.
  @rtype: int
  @return: A valid generic file index on success, 0 otherwise.'''
  pass

def precache_model(model, preload=False):
  '''precache_model(model[, preload=False]) -> int
  
  Precaches a given model.
  
  @type  model: str
  @param model: The name of the model to precache.
  @type  preload: bool
  @param preload: If True, the file will be precached before level startup.
  @rtype: int
  @return: A valid model index on success, 0 otherwise.'''
  pass

def precache_sentence_file(sentence_file, preload=False):
  '''precache_sentence_file(sentence_file[, preload=False]) -> int
  
  Precaches a given sentence file.
  
  @type  sentence_file: str
  @param sentence_file: The name of the sentence file to precache.
  @type  preload: bool
  @param preload: If True, the file will be precached before level startup.
  @rtype: int
  @return: A valid sentence file index on success, 0 otherwise.'''
  pass

def precache_sound(sound, preload=False):
  '''precache_sound(sound[, preload=False]) -> bool
  
  Precaches a given sound.
  
  @type  sound: str
  @param sound: The name of the sound to precache.
  @type  preload: bool
  @param preload: If True, the file will be precached before level startup.
  @rtype: bool
  @return: True on success, False otherwise.'''
  pass

