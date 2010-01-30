======================================================
:mod:`sourcemod.forwards` --- Collections of Callbacks
======================================================

..  module:: sourcemod.forwards
..  moduleauthor:: Zach "theY4Kman" Kanzler <they4kman@gmail.com>

This module exposes a forwards system to its own plug-ins. The system is separate from the forwards system in SourceMod -- that is, you can't create a forward in Viper and have it call SourcePawn functions.

Module Contents
===============

..  _exec-types:

..  list-table::
    :widths: 10 90
    
    * - **Exec Type**
      - **Description**
    * - ..  data:: ET_Ignore
      - Ignore all return values and return None
    * - ..  data:: ET_Single
      - This is no longer used and will act like ET_Ignore
    * - ..  data:: ET_Event
      - Acts as an event, allowing callbacks to return :ref:`Plugin result types <result-types>`. Mid-stops are not allowed, meaning returning :data:`Plugin_Stop <sourcemod.Plugin_Stop>` will not stop the forward. Returns the highest number value returned.
    * - ..  data:: ET_Hook
      - Acts as a hook, allowing callbacks to return :ref:`Plugin result types <result-types>`. Mid-stops are allowed, meaning returning :data:`Plugin_Stop <sourcemod.Plugin_Stop>` will stop the forward. Returns the highest number value returned.
    * - ..  data:: ET_LowEvent
      - Same as ET_Event, except that it returns the lowest number value returned.

..  function:: create(name, callback, et, ...)
    
    Creates a new forward. All the arguments after ``et`` should be the types of the objects that will be passed to the forward's hooks. For example::
        
        >>> myforward = create("", None, ET_Ignore, int, int, str)
        <anonymous Forward: 0x819c12d>
        >>> def myhook(num1, num2, name):
        ...     print num1, num2, name
        ...     return Plugin_Continue
        ... 
        >>> myforward.add_function(myhook)
        >>> myforward.fire(3, 1337, "elite")
        3 1337 elite
    
    To reduce ambiguity, the callbacks that are called when ``myforward.fire()`` is run (added by ``myforward.add_function()``) are called `hooks` or `registered callbacks`.
    
    ..  note::
        
        Pass a blank forward name to create an anonymous forward.
    
    :type   name: str
    :param  name: The name of the new forward.
    :type   callback: callable
    :param  callback: A function that will be called every time a callback registered to this forward returns when it is fired. This callable should have two parameters: the return value of the registered callback, and the actual registered callback function object -- in that order. Pass None to ignore this parameter.
    :type   et: :ref:`ET constant <exec-types>`
    :param  et: How return values of registered callbacks should be handled when the forward is fired.
    :param  ...: The types of the parameters that will be passed to all the registered callbacks when the forward is fired.
    :returns: A :ref:`Forward object <forward-objects>`

..  function:: register(forward, callback)
    
    Registers a callback for the global forward specified in ``forward``.
    
    :returns: True if successful; False if the specified forward could not be found or if the forward name passed is blank.


..  _forward-objects:

Forward Objects
===============

Calling ``len()`` on a forward will return the number of hooks registered to the forward. Also, you can use ``myforward[x]`` to retrieve the registered callback at index ``x``, and use the ``in`` syntax: ``if myhook in myforward``

..  attribute:: name
    
    The name of the forward. This is blank for anonymous forwards.

..  method:: Forward.add_function(func)
    
    Adds a function to the forward's registered callback list. ``func`` should be able to handle all of the arguments of the forward, but that is **NOT** checked automatically.

..  method:: Forward.fire(...)
    
    Fires the forward, passing all arguments to the registered callbacks of the forward. The argument types will be checked against the types passed on creation of the forward. The return type depends on the exec type of the forward.

..  method:: Forward.get_function_count()
    
    Returns the number of callbacks registered to this forward. This is the same as ``len(myforward)``.

..  method:: Forward.remove_function(func)
    
    Removes the first instance of ``func`` registered to this forward. Returns True if the function was found and removed, but False if the function was not in the registered callbacks list.
