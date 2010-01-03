=======================================
:mod:`sourcemod.events` --- Game Events
=======================================

..  module:: sourcemod.events
..  moduleauthor:: Zach "theY4Kman" Kanzler <they4kman@gmail.com>

The Source Engine defines events that are fired when certain things happen. For example, the event ``player_shoot`` is fired when a player fires their weapon in Counter-Strike: Source.

Module Contents
===============

..  _event-hook-modes:

..  list-table::
    :widths: 10 90
    
    * - **Event Hook Mode**
      - **Description**
    * - ..  data:: EventHookMode_Pre
      - Hook callback fired before the event is fired.
    * - ..  data:: EventHookMode_Post
      - Hook callback fired after the event is fired.
    * - ..  data:: EventHookMode_PostNoCopy
      - Hook callback fired after event is fired, but event data is not copied.

..  function:: create(name[, force=False])
    
    Creates a game event. If ``force`` is True, it forces the event to be created even if it's not being hooked. Note that this will not force the event to be created if the event doesn't exist at all.
    
    Returns an :ref:`Event object <event-objects>` that represents the game event created, or None if the event does not exist.

..  function:: hook(event, callback[, mode=:data:`EventHookMode_Post <sourcemod.events.EventHookMode_Post>`])
    
    Hooks a game event. This raises a :exc:`ViperError <sourcemod.ViperError>` if the game event does not exist.

..  function:: unhook(event, callback[, mode=:data:`EventHookMode_Post <sourcemod.events.EventHookMode_Post>`])
    
    Unhooks the specified ``callback`` from a game event. Raises a :exc:`ViperError <sourcemod.ViperError>` if the specified event does not exist or the callback was invalid.


..  _event-objects:

Event Objects
=============

..  note::
    
    There is no way to determine what fields are available in a game event, or the type of a field, which is why this class contains so many inconvenient methods.

..  attribute:: Event.dont_broadcast
    
    *Read-only*. Whether or not this event will be broadcast to players. 

..  attribute:: Event.name
    
    *Read-only*. The name of this game event.

..  method:: Event.cancel()
    
    Cancels this created event.

..  method:: Event.fire([dont_broadcast=False])
    
    Fires a created event. If ``dont_broadcast`` is True, the event is broadcast to the clients.

..  method:: Event.get_bool(field)
             Event.get_float(field)
             Event.get_int(field)
             Event.get_string(field)
    
    Retrieves a value from a game event.
    
    ..  note::
        
        These will *ALWAYS* return a value, even if the field does not exist. Use :meth:`has_field <sourcemod.events.Event.has_field>` to make sure the field exists.

..  method:: Event.has_field(field)
    
    Returns whether or not a field exists on a game event.

..  method:: Event.set_bool(field, value)
             Event.set_float(field, value)
             Event.set_int(field, value)
             Event.set_string(field, value)
    
    Sets a value in a game event.

