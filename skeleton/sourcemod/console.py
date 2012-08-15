FCVAR_ARCHIVE = 128
FCVAR_ARCHIVE_XBOX = 16777216
FCVAR_CHEAT = 16384
FCVAR_CLIENTDLL = 8
FCVAR_DATACACHE = 524288
FCVAR_DEMO = 65536
FCVAR_DONTRECORD = 131072
FCVAR_FILESYSTEM = 2097152
FCVAR_GAMEDLL = 4
FCVAR_INPUTSYSTEM = 33554432
FCVAR_LAUNCHER = 2
FCVAR_MATERIAL_SYSTEM = 16
FCVAR_NETWORKSYSTEM = 67108864
FCVAR_NEVER_AS_STRING = 4096
FCVAR_NONE = 0
FCVAR_NOTIFY = 256
FCVAR_NOT_CONNECTED = 4194304
FCVAR_PLUGIN = 262144
FCVAR_PRINTABLEONLY = 1024
FCVAR_PROTECTED = 32
FCVAR_REPLICATED = 8192
FCVAR_SOUNDSYSTEM = 8388608
FCVAR_SPONLY = 64
FCVAR_STUDIORENDER = 32768
FCVAR_TOOLSYSTEM = 1048576
FCVAR_UNLOGGED = 2048
FCVAR_UNREGISTERED = 1
FCVAR_USERINFO = 512
FCVAR_VPHYSICS = 134217728
SM_REPLY_CHAT = 1
SM_REPLY_CONSOLE = 0

class ConCommandReply:
  '''Stores information about a ConCommand when it is executed.'''
  
  '''The arguments passed when the ConCommand was executed'''
  args = None
  '''The full argument string sent to the server. This includes any quotes sent.'''
  argstring = None
  '''The Client whom executed the ConCommand'''
  client = None
  '''The name of the ConCommand executed.'''
  name = None
  '''How the console command was run: chat or console. Will be an SM_REPLY constant.'''
  reply_to = None
  def reply(self, message):
    '''reply(message)
    
    Replies to the client whom executed the ConCommand in the way they
    executed the ConCommand (console or chat).
    @type  message: string
    @param message: The message to send'''
    pass
  

class ConVar:
  '''Represents a ConVar.'''
  
  '''The bitstring of FCVAR_* flags on this console variable.'''
  flags = None
  '''The lower bound of this ConVar.'''
  lower_bound = None
  '''The name of the ConVar.'''
  name = None
  '''The upper bound of this ConVar.'''
  upper_bound = None
  '''The value of the ConVar.'''
  value = None
  def hook_change(self, callback):
    '''hook_change(callback)
    
    Creates a hook for when a console variable's value is changed.
    @type  callback: callable
    @param callback: The function to call when the ConVar is changed.
        The callback should have the prototype callback(cvar, oldvalue,
        newvalue), where cvar is the ConVar object representing the ConVar
        that was changed, newvalue is the value being assigned to the Convar
        as a string, and oldvalue is the previous value of the ConVar as a string.'''
    pass
  
  def reset(self):
    '''reset()
    
    Resets the console variable to its default value.'''
    pass
  
  def unhook_change(self, callback):
    '''unhook_change(callback)
    
    Removes a ConVar change hook.
    @type  callback: callable
    @param callback: The hook to remove.
    @throw ViperException: No active hook on the ConVar, or an invalid
        or unregistered callback supplied.'''
    pass
  

def create_convar(name, value, description=None, flags=None, min=None, max=None):
  '''create_convar(name, value[, description[, flags[, min[, max]]]]) -> ConVar
  
  Creates a new console variable.
  @type  name: string
  @param name: Name of the ConVar
  @type  value: string
  @param value: The value of the ConVar. As the internal representation
      of a ConVar's value is a string, stringization is left to the user.
  @type  description: string
  @param description: (Optional) Description of the ConVar
  @type  flags: FCVAR constants
  @param flags: (Optional) Flags that change how a ConVar is handled.
      Use FCVAR constants, such as FCVAR_CHEAT, etc.
  @type  min: float
  @param min: The lowest number this ConVar can be set to. Pass None to
      ignore this field.@type  max: float
  @param max: The highest number this ConVar can be set to. Pass None to
      ignore this field.'''
  pass

def find_convar(name):
  '''find_convar(name) -> ConVar
  
  Finds the specified ConVar.
  @type  name: str
  @param name: The name of the ConVar to retrieve.
  @rtype: ConVar
  @return: A valid ConVar object on success, or None if the ConVar could not be
      found'''
  pass

def print_to_server(message):
  '''print_to_server(message)
  
  Sends a message to the server console.
  @type  message: string
  @param message: The message to print'''
  pass

def reg_concmd(name, callback, description=None, flags=None):
  '''reg_concmd(name, callback[, description[, flags]]) -> bool
  
  Registers a new console command or hooks an existing one.
  @type  name: string
  @param name: Name of the ConCommand
  @type  callback: callable
  @param callback: A function to call when the ConCommand is executed.
      The function should receive one argument: a console.ConCommandReply object.
  @type  description: string
  @param description: (Optional) Description of the ConCommand
  @type  flags: FCVAR constants
  @param flags: (Optional) Flags that change how a ConCommand is handled.
      Use FCVAR constants, such as FCVAR_CHEAT, etc.'''
  pass

def reg_srvcmd(name, callback, description=None, flags=None):
  '''reg_srvcmd(name, callback[, description[, flags]]) -> bool
  
  Registers a new server console command or hooks an existing one.
  @type  name: string
  @param name: Name of the ConCommand
  @type  callback: callable
  @param callback: A function to call when the ConCommand is executed.
      The function should receive one argument: a console.ConCommandReply object.
  @type  description: string
  @param description: (Optional) Description of the ConCommand
  @type  flags: FCVAR constants
  @param flags: (Optional) Flags that change how a ConCommand is handled.
      Use FCVAR constants, such as FCVAR_CHEAT, etc.'''
  pass

def server_command(command):
  '''server_command(command)
  
  Executes a command as if it were on the server console
  @type  command: string
  @param command: Command to execute'''
  pass

def server_execute():
  '''server_execute()
  
  Executes every command in the server's command buffer now, rather than once
  per frame.'''
  pass

