==============================================
:mod:`sourcemod.keyvalues` --- Valve KeyValues
==============================================

..  module:: sourcemod.keyvalues
..  moduleauthor:: Zach "theY4Kman" Kanzler <they4kman@gmail.com>

KeyValues are defined recursively as sections and pairs of named keys and values. See the `VDC Article on KeyValues <http://developer.valvesoftware.com/wiki/KeyValues>`_

Module Contents
===============

..  function:: keyvalues_from_file(file [, use_escape_sequences=False])
    
    Creates a new :class:`KeyValues object <sourcemod.keyvalues.KeyValues>` from a file or file-like object.
    
    :type   file: str/file
    :param  file: If a string, creates a new KeyValues object from the filename stored in the string. Otherwise it must be a file-like object, in that it must have a ``read()`` function.
    :type   use_escape_sequences: bool
    :param  use_escape_sequences: Whether or not to parse escape sequences such as ``\n`` or ``\\``
    :rtype: :class:`KeyValues <sourcemod.keyvalues.KeyValues>`
    :return: A valid :class:`KeyValues object <sourcemod.keyvalues.KeyValues>` on success, None otherwise.


..  index:: KeyValues

..  class: KeyValues

KeyValues Objects
=================

KeyValues objects are essentially dictionaries, and thus are accessed in the same way. They can be instantiated by passing a dictionary, which is then transformed behind the scenes into a Valve KeyValues object.::
    
    from sourcemod.keyvalues import KeyValues
    kv = KeyValues('root_section_name', {
        'key1': 'value1',
        'subsection1': {
            'subkey1': 'subvalue1',
            'subkey2': 2,
            'subkey3': 4.7123
          }
      })

::

    >>> kv['key1']
    'value1'
    >>> kv['subsection1']
    subsection1
    {
        "subkey1"     "subvalue1"
        "subkey2"     "2"
        "subkey3"     "4.7123"
    }
    >>> kv['subsection1']['subkey2']
    2
    >>> kv['subsection1']['subkey2'] = 83
    83
    >>> kv['subsection1']
    subsection1
    {
        "subkey1"     "subvalue1"
        "subkey2"     "83"
        "subkey3"     "4.7123"
    }

Attributes and Methods
----------------------

..  data:: KeyValues.name
    
    The root section name of this KeyValues.

..  data:: KeyValues.uses_escape_sequences
    
    Whether or not to parse escape sequences such as ``\n`` or ``\\``

..  method:: KeyValues.clear([key])
    
    With no arguments, removes all sub-keys. With `key`, this clears the value of ``kv[key]``, turning it into an empty section. If `key` is already a section, this is the same as ``kv[key].clear()``
    
    :type   key: str
    :param  key: The name of the key to clear.

..  method:: KeyValues.copy()
    
    Deep copies the current KeyValues into a completely new KeyValues.
    
    :rtype: KeyValues
    :return: A new KeyValues object with the same structure as this KeyValues.

..  method:: KeyValues.parse(string)
    
    Parses a KeyValues structure from a string into the KeyValues object.
    
    :type   string: str
    :param  string: The string value to parse

..  method:: KeyValues.save(file)
        
    Save this KeyValues to a file
    
    :type   file: str or file
    :param  file: A :keyword:`file` object to save to. 


The KeyValues object supports the dictionary object protocol, so almost everything you can do with a :keyword:`dict`, you can do with a KeyValues.

..  method:: KeyValues.iteritems()
             KeyValues.iterkeys()
             KeyValues.itervalues()
    
    Iterators for items (key and value in a tuple), keys, or values.

..  method:: KeyValues.items()
             KeyValues.keys()
             KeyValues.values()
    
    Returns a list of items (key and value in a tuple), keys, or values.

..  method:: KeyValues.get(key[, default=None])
    
    If key exists in the KeyValues, this returns that value. If it does not exist, it returns what is passed to ``default``. The benefit of this function is that it never raises a KeyError.

..  describe:: len(kv)
    
    Returns the number of items in the KeyValues *kv*

..  describe:: kv[key]
    
    Returns the value of key *key* from KeyValues *kv*

..  describe:: kv[key] = value
    
    Set ``kv[key]`` to *value*.

..  describe:: del kv[key]
    
    Remove ``kv[key]`` from *kv*. Raises a :exc:`KeyError` if *key* is not in the KeyValues.

..  describe:: key in kv
    
    Return ``True`` if *kv* has a key *key*, else ``False``

..  describe:: key not in kv
    
    Equivalent to ``not key in kv``

..  describe:: iter(kv)
    
    Return an iterator over the keys of the KeyValues. This is a shortcut for :meth:`iterkeys <sourcemod.keyvalues.KeyValues.iterkeys>`