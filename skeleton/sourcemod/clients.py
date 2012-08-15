BANFLAG_AUTHID = 4
BANFLAG_AUTO = 1
BANFLAG_IP = 2
BANFLAG_NOKICK = 8
BANFLAG_NOWRITE = 16
BANTIME_FOREVER = 0
COMMAND_FILTER_ALIVE = 1
COMMAND_FILTER_CONNECTED = 4
COMMAND_FILTER_DEAD = 2
COMMAND_FILTER_NO_BOTS = 32
COMMAND_FILTER_NO_IMMUNITY = 8
COMMAND_FILTER_NO_MULTI = 16
COMMAND_TARGET_AMBIGUOUS = -7
COMMAND_TARGET_EMPTY_FILTER = -5
COMMAND_TARGET_IMMUNE = -4
COMMAND_TARGET_NONE = 0
COMMAND_TARGET_NOT_ALIVE = -1
COMMAND_TARGET_NOT_DEAD = -2
COMMAND_TARGET_NOT_HUMAN = -6
COMMAND_TARGET_NOT_IN_GAME = -3
COMMAND_TARGET_VALID = 1

class Client:
  '''Contains methods and members to manipulate a player.'''
  
  '''The client's angles vector.
  @throw ViperError: Invalid client, client not in-game, or no mod support.'''
  abs_angles = None
  '''The client's origin vector.
  @throw ViperError: Invalid client, client not in-game, or no mod support.'''
  abs_origin = None
  '''Whether the client is alive or dead.
  @throw ViperError: Invalid client, client not in-game, or no mod support.'''
  alive = None
  '''The client's armor value.
  @throw ViperError: Invalid client, client not in-game, or no mod support.'''
  armor = None
  '''The client's average packet choke. Value is a percentage ranging from 0.0 to 1.0
  @throw ViperError: Invalid client, client not in-game, or fake client.'''
  avg_choke = None
  '''The client's average incoming packet choke. Value is a percentage ranging from
  0.0 to 1.0
  @throw ViperError: Invalid client, client not in-game, or fake client.'''
  avg_choke_in = None
  '''The client's average outgoing packet choke. Value is a percentage ranging from
  0.0 to 1.0
  @throw ViperError: Invalid client, client not in-game, or fake client.'''
  avg_choke_out = None
  '''The client's data flow, incoming and outgoing, in bytes per second.
  @throw ViperError: Invalid client, client not in-game, or no mod support.'''
  avg_data = None
  '''The client's incoming data flow in bytes per second.
  @throw ViperError: Invalid client, client not in-game, or no mod support.'''
  avg_data_in = None
  '''The client's outgoing data flow in bytes per second.
  @throw ViperError: Invalid client, client not in-game, or no mod support.'''
  avg_data_out = None
  '''The client's average packet latency, both incoming and outgoing, in seconds.
  @throw ViperError: Invalid client, client not in-game, or fake client.'''
  avg_latency = None
  '''The client's average incoming packet latency in seconds.
  @throw ViperError: Invalid client, client not in-game, or fake client.'''
  avg_latency_in = None
  '''The client's average outgoing packet latency in seconds.
  @throw ViperError: Invalid client, client not in-game, or fake client.'''
  avg_latency_out = None
  '''The client's average packet loss, incoming and outgoing. Value is a percentage
  ranging from 0.0 to 1.0
  @throw ViperError: Invalid client, client not in-game, or fake client.'''
  avg_loss = None
  '''The client's average incoming packet loss. Value is a percentage ranging from0.0 to 1.0
  @throw ViperError: Invalid client, client not in-game, or fake client.'''
  avg_loss_in = None
  '''The client's average outgoing packet loss. Value is a percentage ranging from
  0.0 to 1.0
  @throw ViperError: Invalid client, client not in-game, or fake client.'''
  avg_loss_out = None
  '''The client's average packet frequency, incoming and outgoing, in packets per
  second.
  @throw ViperError: Invalid client, client not in-game, or fake client.'''
  avg_packets = None
  '''The client's average incoming packet frequency in packets per second.
  @throw ViperError: Invalid client, client not in-game, or fake client.'''
  avg_packets_in = None
  '''The client's average outgoing packet frequency in packets per second.
  @throw ViperError: Invalid client, client not in-game, or fake client.'''
  avg_packets_out = None
  '''The client's send data rate in bytes per second.
  @throw ViperError: Invalid client, client not in-game, or fake client.'''
  data_rate = None
  '''The client's death count.
  @throw ViperError: Invalid client, client not in-game, or no mod support.'''
  deaths = None
  '''The Entity of the client.
  @throw ViperError: Invalid client, client not connected, or client not in-game.'''
  entity = None
  '''Whether the client is fake or not.
  @throw ViperError: Invalid client or client not connected.'''
  fake = None
  '''The client's frag count.
  @throw ViperError: Invalid client, client not in-game, or no mod support.'''
  frags = None
  '''The health of the client.
  @throw ViperError: Invalid client, client not in-game, or no mod support.'''
  health = None
  '''The client index of the client'''
  index = None
  '''The IP address of this client.
  @throw ViperError: Invalid client or client not connected.'''
  ip = None
  '''This client's language id.
  @throw ViperError: Invalid client or client not connected.'''
  lang_id = None
  '''The client's packet latency (RTT), both incoming and outgoing, in seconds.
  This is more accurate than the average latency, but jittery.
  @throw ViperError: Invalid client, client not in-game, or fake client.'''
  latency = None
  '''The client's incoming packet latency (RTT) in seconds.
  This is more accurate than the average latency, but jittery.
  @throw ViperError: Invalid client, client not in-game, or fake client.'''
  latency_in = None
  '''The client's outgoing packet latency (RTT) in seconds.
  This is more accurate than the average latency, but jittery.
  @throw ViperError: Invalid client, client not in-game, or fake client.'''
  latency_out = None
  '''The client's max size vector.
  @throw ViperError: Invalid client, client not in-game, or no mod support.'''
  maxs = None
  '''The client's min size vector.
  @throw ViperError: Invalid client, client not in-game, or no mod support.'''
  mins = None
  '''The client's model name.
  @throw ViperError: Invalid client, client not in-game, or no mod support.'''
  model = None
  '''The name of this client.
  @throw ViperError: Invalid client or client not connected.'''
  name = None
  '''The client's unique serial identifier.'''
  serial = None
  '''The Steam ID of this client. This is None when the client is not
  authorized yet.
  @throw ViperError: Invalid client or client not connected.'''
  steamid = None
  '''This player's team.
  @throw ViperError: Invalid client, client not connected, or client not in-game.'''
  team = None
  '''The amount of time the client has been connected in seconds.
  @throw ViperError: Invalid client, client not in-game, or fake client.'''
  time_connected = None
  '''The userid of this client.
  @throw ViperError: Invalid client or client not connected.'''
  userid = None
  '''The client's weapon name.
  @throw ViperError: Invalid client, client not in-game, or no mod support.'''
  weapon = None
  def ban(self, time, flags, reason, kickmsg="Kicked", cmd=None, source=0):
    '''ban(time, flags, reason[, kickmsg="Kicked"[, cmd=None[, source=0]]]) -> bool
    
    Bans the client.
    
    @type  time: int
    @param time: Time, in minutes, to ban (0 = permanent)
    @type  flags: banning.BANFLAG
    @param flags: Flags for controlling the ban mechanism. If BANFLAG_AUTHID is
        set and no AUTHID is available, the ban will fail unless AUTO is also
        flagged.
    @type  reason: str
    @param reason: Reason to ban the client for.
    @type  kickmsg: str
    @param kickmsg: Message to display to the user when they're kicked.
    @type  cmd: str
    @param cmd: Command string to identify the source. If this is left empty,
        then the ban_client forward will not be called.
    @type  source: object
    @param source: A source value that could be interpreted as the identity of the
        player whom was the source of the banning (not actually checked by Core).
    @rtype: bool
    @return: True on success, False on failure.'''
    pass
  
  def fake_command(self, cmd):
    '''fake_command(cmd)
    
    Executes a client command on the server without being networked.
    
    @type  cmd: str
    @param cmd: The command string to execute.'''
    pass
  
  def is_connected(self):
    '''is_connected() -> bool
    
    Returns whether the client is connected.
    @rtype: bool
    @return: True if client is connected, False if not.'''
    pass
  
  def is_timing_out(self):
    '''is_timing_out() -> bool
    
    Returns if the client is timing out.
    
    @rtype: bool
    @return: True if the client is timing out, False otherwise.'''
    pass
  
  def kick(self, msg="", delay=True):
    '''kick([msg=""[, delay=True]])
    
    Disconnects a player from a server.
    
    @type  msg: str
    @param msg: A message to show the user as a disconnect reason. Note that a
       period is automatically appended to the message by the engine.
    @type  delay: bool
    @param delay: If True, the client is kicked in the next game frame. If False,
       the client is kicked immediately. The delay exists to prevent accidental
       engine crashes.'''
    pass
  
  def notify_post_admin_check(self):
    '''notify_post_admin_check()
    
    Signals that a player has completed post-connection admin checks. Has no effect
    if the player has already had this event signalled. Note: This must be sent even
    if no admin id was assigned.'''
    pass
  
  def print_center(self, message):
    '''print_center(message)
    
    Prints a message to this client in the center of the screen.
    @type  message: str
    @param message: The message to print'''
    pass
  
  def print_chat(self, message):
    '''print_chat(message)
    
    Prints a message to this client's chat area.
    @type  message: str
    @param message: The message to print'''
    pass
  
  def print_console(self, message):
    '''print_console(message)
    
    Prints a message to this client's console.
    
    @type  message: str
    @param message: The message to print'''
    pass
  
  def print_hint(self, message):
    '''print_hint(message)
    
    Prints a message to this client with a hint box.
    
    @type  message: str
    @param message: The message to print'''
    pass
  
  def set_fake_client_convar(self, convar, value):
    '''set_fake_client_convar(convar, value)
    
    Sets a convar value on a fake client.
    @type  convar: str
    @param convar: The ConVar name.
    @type  value: str
    @param value: The value to set the ConVar to.'''
    pass
  
  def show_vgui_panel(self, name, kv=None, show=True):
    '''show_vgui_panel(name[, kv=None[, show=True]])
    
    Shows a VGUI panel to a specific client
    
    @type  name: str
    @param name: Panel type name (see viewport_panel_names.h for a list of panel
       names)
    @type  kv: sourcemod.keyvalues.KeyValues
    @param kv: A KeyValues with all the data for the panel setup. The data inside
       the KeyValues depends on the panel type.
    @type  show: bool
    @param show: True to show the panel, or False to remove it from the client's
        screen'''
    pass
  

def create_fake_client(name):
  '''create_fake_client(name) -> Client object
  
  Creates a fake client.
  @type  name: str
  @param name: The name to use for the fake client
  @rtype: sourcemod.clients.Client
  @return: A valid Client object on success, None otherwise.'''
  pass

def get_client(index):
  '''get_client(index) -> Client object
  
  Retrieves the Client object of that client index
  @type  index: int
  @param index: The client index to find the Client object for
  @rtype: sourcemod.clients.Client
  @return: A valid Client object on success, None if an invalid client index.'''
  pass

def get_client_count(in_game_only=True):
  '''get_client_count([in_game_only=True]) -> int
  
  Returns the number of clients put in the server
  @type  in_game_only: bool
  @param in_game_only: If False, players currently connecting are also counted.'''
  pass

def get_client_from_serial(serial):
  '''get_client_from_serial(serial) -> Client object
  
  Retrieves a Client from its serial number.
  
  @type  serial: int
  @param serial: The Client's serial number
  @rtype: sourcemod.clients.Client
  @return: A valid Client object on success, None if an invalid serial.'''
  pass

def get_client_from_userid(userid):
  '''get_client_of_userid(userid) -> Client object
  Translates a userid index to a Client object
  @type  userid: int
  @param userid: Userid value
  @rtype: sourcemod.clients.Client
  @return: A valid Client object on success, None if an invalid userid.'''
  pass

def get_max_clients():
  '''get_max_clients() -> int
  
  Returns the maximum number of clients allowed on the server. This may return 0
  if called before on_map_start.'''
  pass

def process_target_string(pattern, admin, flags=0):
  '''process_target_string(pattern, admin[, flags=0]) -> tuple
  
  Processes a generic target string and resolves it to a list of clients or one
  client, based on filtering rules.
  @type  pattern: str
  @param pattern: Target pattern to process
  @type  admin: clients.Client or int
  @param admin: The clients.Client object or client index of the client to
      process the target string from
  @type  flags: int
  @param flags: A combination of COMMAND_FILTER flags. See clients.COMMAND_FILTER*
  @rtype: tuple
  @return: A tuple containing the useful values:
      (targets, target_name, tn_is_ml, reason)
       targets: a list of Client objects that matched the pattern
       target_name: a string describing the player(s) matched
       tn_is_ml: a boolean that's True if 'target_name' is multi-lingual
       reason: an int containing a clients.COMMAND_TARGET value'''
  pass

