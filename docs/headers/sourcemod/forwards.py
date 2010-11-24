ET_Event = 2
ET_Hook = 3
ET_Ignore = 0
ET_LowEvent = 4
ET_Single = 1

class Forward:
  '''Contains methods to manipulate a forward.'''
  
  '''The name of the forward.'''
  name = None
  def add_function(self, func):
    '''add_function(func)
    
    Adds a function to the the forward's function list.
    @type  func: callable
    @param func: The function to add to the list. The function should be able
       to handle all the arguments of the forward, though that is NOT checked
       automatically.'''
    pass
  
  def fire(self, arg1, *args):
    '''fire([arg1[, ... ]]) -> int
    
    Fires the forward, passing all arguments passed to fire() to the callbacks
    @rtype: int
    @return: Depends on the ExecType of the forward.'''
    pass
  
  def get_function_count():
    '''Returns the number of callbacks registered on this forward
    @rtype: int
    @return: Length of the callbacks list'''
    pass
  
  def remove_function(self, func):
    '''remove_function(func) -> bool
    
    Removes the first instance of the function from the function list.
    @type  func: callable
    @param func: The function to remove
    @rtype: bool
    @return: True if the function was found and removed, False if the function
       was not in the function list.'''
    pass
  

def create(name, callback, et, *args):
  '''create(name, callback, et, ...) -> Forward object
  
  Creates a new forward. All the arguments after |et| are the types of the objects
  that will be passed to the forward's hooks. For example:
  
     >>> myforward = create("", None, ET_Ignore, int, int, str)
  <anonymous Forward: 0x819c12d>
     >>> def myhook(num1, num2, name):
     >>>     print num1, num2, name   >>>     return Plugin_Continue
  
     >>> myforward.add_function(myhook)
     >>> myforward.fire(3, 1337, "elite")
  3 1337 elite
  
  @note: Pass a blank forward name to create an anonymous forward.
  @type  name: string
  @param name: The name of the new forward
  @type  callback: callable
  @param callback: A function that will be called every time a function returns
     when the forward is fired. The callable should have one parameter: the return
     value of the callback. Pass None to ignore this parameter.
  @type  et: ET constant
  @param et: How return values of functions are handled when the forward is fired.
     Use ET_Ignore, ET_Single, ET_Event, ET_Hook, or ET_LowEvent
  @type  ...: type
  @param ...: The types of the parameters that will be passed to all the hooks
     when the forward is fired.@rtype: sourcemod.forwards.Forward
  @return: A Forward object that may manipulate the forward.'''
  pass

def register(forward, callback):
  '''register(forward, callback) -> bool
  
  Registers a callback for the specified global forward.
  @type  forward: string
  @param forward: The name of the global forward
  @type  callback: callable
  @param callback: The function to be called when the forward is fired.
  @rtype: bool
  @return: True if successful, false if the specified forward could not be found,
     or the forward name passed is blank.'''
  pass

