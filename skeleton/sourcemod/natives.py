
class FloatRef:
  '''A reference to a float in a SourcePawn runtime'''
  
  '''The number of cells allocated for this reference.'''
  size = None
  '''The value of the reference.'''
  value = None

class Ref:
  '''A reference to memory in a SourcePawn runtime'''
  
  '''The number of cells allocated for this reference.'''
  size = None
  '''The value of the reference.'''
  value = None

class StringRef:
  '''A reference to a string in a SourcePawn runtime'''
  
  '''The number of cells allocated for this reference.'''
  size = None
  '''The value of the reference.'''
  value = None

def invoke(native, *args):
  '''invoke(native, ...) -> int
  
  Calls a SourceMod native.
  @type  native: str
  @param native: The name of the native to invoke.
  @rtype: int
  @return: The cell value returned by the native.
  @throw ViperError: Native does not exist.
  @throw ViperError: Native invocation failed.'''
  pass

def invoke_float(native, *args):
  '''invoke_float(native, ...) -> float
  
  Calls a SourceMod native and casts its return value to float.
  @type  native: str
  @param native: The name of the native to invoke.
  @rtype: float
  @return: The float value returned by the native.
  @throw ViperError: Native does not exist.
  @throw ViperError: Native invocation failed.'''
  pass

