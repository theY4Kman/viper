EventHookMode_Post = 1
EventHookMode_PostNoCopy = 2
EventHookMode_Pre = 0

class Event:
  '''Contains methods to manipulate a game event.'''
  
  '''Whether or not this event will be broadcast to players.'''
  dont_broadcast = None
  '''The name of this game event.'''
  name = None
  def cancel(self):
    '''cancel()
    
    Cancels a created event.'''
    pass
  
  def fire(self, dont_broadcast=False):
    '''fire([dont_broadcast=False])
    
    Fires a created event.
    
    @type  dont_broadcast: bool
    @param dont_broadcast: Determines whether or not to broadcast this event to
       the clients.'''
    pass
  
  def get_fields(self):
    '''get_fields() -> dict
    
    Returns a dict with the game event's fields as keys and types as values.
    The values will be strings as seen in modevents.res.'''
    pass
  
  def is_empty(self, field):
    '''is_empty(field) -> bool
    
    Returns whether or not an event field has an empty value.
    
    @type  field: str
    @param field: The field to check
    @rtype: bool
    @return: True if the field has a value, False if not.'''
    pass
  

def create(name, force=False):
  '''create(name[, force=False]) -> Event object
  
  Creates a game event.
  @type  name: str
  @param name: The name of the event to create
  @type  force: bool
  @param force: Forces event to be created, even it's not being hooked. Note that
     this will not force it if the event doesn't exist at all.
  @rtype: sourcemod.events.Event
  @return: The Event object that represents the game event created, or None if
     the event does not exist.'''
  pass

def hook(event, callback, mode=EventHookMode_Post):
  '''hook(event, callback[, mode=EventHookMode_Post])
  
  Hooks a game event. Throws an exception if the game event does not exist.
  @type  event: str
  @param event: The game event to hook, e.g., "player_death"
  @type  callback: callable
  @param callback: The function to call when the event is fired.
  @type  mode: EventHookMode constant
  @param mode: Changes when and how the hook will be fired:
     - EventHookMode_Pre: callback fired before event is fired
     - EventHookMode_Post: callback fired after event is fired
     - EventHookMode_PostNoCopy: callback fired after event is fired, but event
       data won't be copied.
  @throw ViperException: Event does not exist
  @throw ViperException: Invalid callback'''
  pass

def unhook(event, callback, mode=EventHookMode_Post):
  '''unhook(event, callback[, mode=EventHookMode_Post])
  
  Unhooks a callback from a game event.
  @see: sourcemod.events.hook
  @throw ViperException: the specified event does not exist, or the callback
     was invalid.'''
  pass

