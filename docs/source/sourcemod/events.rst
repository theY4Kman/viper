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

..  function:: hook(event, callback[, mode=EventHookMode_Post])
    
    Hooks a game event. This raises a :exc:`ViperError <sourcemod.ViperError>` if the game event does not exist. ``mode`` expects an :ref:`EventHookMode constant <event-hook-modes>`.
    
    ``callback`` should be a callable that accepts two arguments: an :ref:`event object <event-objects>`, and the event name as a string -- in that order.

..  function:: unhook(event, callback[, mode=EventHookMode_Post])
    
    Unhooks the specified ``callback`` from a game event. Raises a :exc:`ViperError <sourcemod.ViperError>` if the specified event does not exist or the callback was invalid. ``mode`` expects an :ref:`EventHookMode constant <event-hook-modes>`.


..  _event-objects:

Event Objects
=============

Event fields are accessed using the dictionary syntax::

    evt['field']
    evt['field'] = 'value'

Viper parses the mod's resource/modevents.res file for every field and type for each game event. That allows Viper to return ``evt['field']`` as the correct type for that field. So if in modevents.res there is a "round_start" event with a field "fraglimit" of type "short", Viper will return a Python int for ``evt['fraglimit']``.

Since Viper knows the fields each game event has, it provides length::
    
    len(evt) # The number of fields `evt` has

To iterate over the fields of an event, use the :ref:`get_fields <sourcemod.events.Event.get_fields>` function to retrieve the fields. Then iterate over the dictionary however you please::

    fields = evt.get_fields()
    for field in fields.iterkeys():
        print field
    for field,type in fields.iteritems():
        print "%s is of type %s" % (field, type)

..  attribute:: Event.dont_broadcast
    
    *Read-only*. Whether or not this event will be broadcast to players. 

..  attribute:: Event.name
    
    *Read-only*. The name of this game event.

..  method:: Event.cancel()
    
    Cancels this created event.

..  method:: Event.fire([dont_broadcast=False])
    
    Fires a created event. If ``dont_broadcast`` is True, the event is broadcast to the clients.

..  method:: Event.get_fields()
    
    Retrieves the fields of a game event as seen in modevents.res. They are returned as a dictionary, with the keys being the field names and the values being their type. For example::

    >>> evt.get_fields()
    {'player': 'byte', 'quality': 'byte', 'item': 'string', 'method': 'byte', 'propername': 'bool'}

..  method:: Event.is_empty(field)
    
    Returns whether or not an event field has an empty value.
    
    .. note::
        
        This only tells if an event field has a value. This means that if you create an event, none of its fields will have values, and this function will return True for all of them.

