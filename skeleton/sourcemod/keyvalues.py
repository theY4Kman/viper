
class KeyValues:
  '''Abstracts the Valve KeyValues class.'''
  
  '''The root section name of this KeyValues.'''
  name = None
  '''Whether or not to parse escape sequences such as \n or \".'''
  uses_escape_sequences = None
  def clear(self, key=None):
    '''clear([key])
    
    With no arguments, removes all sub-keys. With |key|, this clears the value of
    kv[key], turning it into an empty section. If |key| is already a section, this
    is the same as kv[key].clear().
    
    @type  key: str
    @param key: The key to clear.'''
    pass
  
  def copy(self):
    '''copy() -> KeyValues
    
    Recursively copies the current KeyValues into a new KeyValues.
    
    @rtype: KeyValues
    @return: A new KeyValues object with the same structure as this KeyValues.'''
    pass
  
  def get(self, key, default=None):
    '''get(key[, default=None]) -> object
    
    If the key exists, returns its value. Otherwise, returns `default`.
    
    @type  key: str
    @param key: The key to lookup
    @type  default: object
    @param default: An object to return in key does not exist.
    @rtype: object
    @return: kv[key] on success, default if key does not exist.'''
    pass
  
  def has_key(self, key):
    '''has_key(key) -> bool
    
    Checks if the KeyValues object contains `key`. Equivalent to `key in kv`
    
    @type  key: str
    @param key: The key to check for
    @rtype: bool
    @return: True if exists, False if it doesn't.'''
    pass
  
  def items(self):
    '''items() -> list
    
    Returns a list of the tuple(key,value) pairs in the KeyValues.'''
    pass
  
  def iteritems(self):
    '''iteritems() -> KeyValuesItemsIterator
    
    Returns an iterator that iterates over all the key/value pairs in the KeyValues.'''
    pass
  
  def iterkeys(self):
    '''iterkeys() -> KeyValuesKeysIterator
    
    Returns an iterator that iterates over all the keys in the KeyValues. This is
    the same as `for key in kv`'''
    pass
  
  def itervalues(self):
    '''itervalues() -> KeyValuesValuesIterator
    
    Returns an iterator that iterates over all the values in the KeyValues.'''
    pass
  
  def keys(self):
    '''keys() -> list
    
    Returns a list of all the keys in the KeyValues.'''
    pass
  
  def parse(self, string):
    '''parse(string)
    
    Parses the string for a KeyValues structure and loads it in.
    
    @type  string: str
    @param string: The string value to parse.'''
    pass
  
  def save(self, file):
    '''save(file)
    
    Saves this KeyValues to a file.
    
    @type  file: str or file
    @param file: A file object to save to.'''
    pass
  
  def values(self):
    '''values() -> list
    
    Returns a list of all the values in the KeyValues.'''
    pass
  

def keyvalues_from_file(file, use_escape_sequences=False):
  '''keyvalues_from_file(file[, use_escape_sequences=False]) -> keyvalues.KeyValues
  
  Creates a new KeyValues object from a file or file-like object.
  
  @type  file: str or file
  @param file: If a string, creates a new KeyValues from the filename pointed to
     by `file`. Otherwise, `file` must be a file-like object -- it must have a
     read() function.
  @type  use_escape_sequences: bool
  @param use_escape_sequences: Whether or not to parse escape sequences such as
     \n or \".@rtype: keyvalues.KeyValues
  @return: A valid KeyValues object on success, None otherwise.'''
  pass

