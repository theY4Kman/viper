Plugin_Continue = 0
Plugin_Handled = 3
Plugin_Stop = 4
server_err = None
server_out = None
stderr = None
stdout = None

class ViperError:
  '''None'''
  
  args = None
  '''exception message'''
  message = None

def get_game_path():
  '''get_game_path() -> str
  
  Returns the full path to the game directory.
  
  @rtype: string
  @return: Returns the path to the game directory'''
  pass

def get_sourcemod_path(path=""):
  '''get_sourcemod_path([path=""]) -> str
  
  Returns the full path to SourceMod.
  
  @type  path: str
  @param path: A path to append to the SourceMod path
  @rtype: string
  @return: Returns the path to SourceMod'''
  pass

