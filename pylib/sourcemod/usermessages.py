USERMSG_BLOCKHOOKS = 128
USERMSG_INITMSG = 8
USERMSG_RELIABLE = 4

def begin():
  '''begin() -> bitbuf.BitBuf
  
  Returns a bitbuf.BitBuf object to be used in sending a usermessage.
  @rtype: sourcemod.bitbuf.BitBuf
  @return: A writable bitbuf.BitBuf object.'''
  pass

def get_message_index(msg_name):
  '''get_message_index(msg_name) -> int
  
  Get the user message index of the user message `msg_name`
  @type  msg_name: str
  @param msg_name: The name of the usermessage to find the index for.
  @rtype: int
  @return: The index of the usermessage on success, -1 otherwise.'''
  pass

def get_message_name(msg_id):
  '''get_message_name(msg_id) -> str
  
  Retrieves the name of a usermessage by its index/ID.
  @type  msg_id: int
  @param msg_id: The index of the usermessage to get the name of.
  @rtype: str
  @return: The name of the usermessage on success, None otherwise.'''
  pass

def hook(usermsg, callback, intercept=False, post_callback=None):
  '''hook(usermsg, callback[, intercept=False[, post_callback=None]]) -> bool
  
  Hooks a usermessage
  @type  usermsg: str or int
  @param usermsg: The name or ID of the usermessage to hook.
  @type  callback: callable
  @param callback: The function to use as a hook.
  @type  intercept: bool
  @param intercept: If True, message will be fully intercepted, allowing the user
      to block the message. Otherwise, the hook is normal and its return value is
      ignored.
  @type  post_callback: callable
  @param post_callback: Optional notification function.
  @rtype: bool
  @return: True if successfully hooked, False otherwise.'''
  pass

def send(usermsg, bitbuffer, clients, flags=0):
  '''send(usermsg, bitbuffer, clients[, flags=0]) -> bool
  
  Sends the usermessage with an ID or name `usermsg` using `bitbuffer`
  as data to all the clients in `clients`, optionally using `flags`.
  @type  usermsg: str or int
  @param usermsg: A string containing the name of the usermessage, or
      an integer containing the usermessage's index.
  @type  bitbuffer: sourcemod.bitbuf.BitBuf
  @param bitbuffer: A bitbuf.BitBuf object that contains all the data
      needed to send the usermessage. This object can be obtained
      with the usermessages.begin() function.
  @type  clients: list
  @param clients: A list of clients to send the usermessage to. The
      list can contain clients.Client objects or integers containing
      indices of clients.
  @type  flags: USERMSG constants
  @param flags: Optional flags to set. See usermessages.USERMSG_*'''
  pass

def unhook(usermsg, callback, intercept=False):
  '''unhook(usermsg, callback[, intercept=False])
  
  Removes one usermessage hook, and its notification callback, if one was set.
  @type  usermsg: str or int
  @param usermsg: A string containing the name of the usermessage, or
      an integer containing the usermessage's index.
  @type  callback: callable
  @param callback: The function used for the hook.
  @type  intercept: bool
  @param intercept: Whether the hook was registered as an intercept hook or not.'''
  pass

