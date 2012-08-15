
class BitBuf:
  '''Interact with writable Source BitBuffers (bf_write).'''
  
  def write_angle(self, angle, numBits=8):
    '''write_angle(angle[, numBits=8])
    
    Writes a bit angle to a writable bitbuffer (bf_write).
    @note: nothing is actually written to a bitbuffer until necessary.
    @type  angle: float
    @param angle: Angle to write to the bitbuffer.
    @type  numBits: int@param numBits: Optional number of bits to use.'''
    pass
  
  def write_angles(self, angles):
    '''write_angles(angles)
    
    Writes a 3D angle vector to a writable bitbuffer (bf_write).
    @note: nothing is actually written to a bitbuffer until necessary.
    @type  angles: datatypes.Vector or list-like[3]
    @param angles: Angles to write to the bitbuffer.'''
    pass
  
  def write_bool(self, bit):
    '''write_bool(bit)
    
    Writes a single bit to a writable bitbuffer (bf_write).
    @note: nothing is actually written to a bitbuffer until necessary.
    @type  bit: bool
    @param bit: A single bit to write to the bitbuffer.'''
    pass
  
  def write_byte(self, byte):
    '''write_byte(byte)
    
    Writes a byte to a writable bitbuffer (bf_write).
    @note: nothing is actually written to a bitbuffer until necessary.
    @type  byte: int
    @param byte: A byte to write to the bitbuffer. Value will be written as 8bit.'''
    pass
  
  def write_char(self, char):
    '''write_char(char)
    
    Writes a character to a writable bitbuffer (bf_write).
    @note: nothing is actually written to a bitbuffer until necessary.
    @type  char: str[1]
    @param char: A character to write to the bitbuffer.'''
    pass
  
  def write_coord(self, coord):
    '''write_coord(coord)
    
    Writes a coordinate to a writable bitbuffer (bf_write).
    @note: nothing is actually written to a bitbuffer until necessary.
    @type  coord: float
    @param coord: Coordinate to write to the bitbuffer.'''
    pass
  
  def write_entity(self, entity):
    '''write_entity(entity)
    
    Writes an entity to a writable bitbuffer (bf_write).
    @note: nothing is actually written to a bitbuffer until necessary.
    @type  entity: entity.Entity or int
    @param entity: Entity object or entity index to write to the bitbuffer.'''
    pass
  
  def write_float(self, num):
    '''write_float(num)
    
    Writes a floating point number to a writable bitbuffer (bf_write).
    @note: nothing is actually written to a bitbuffer until necessary.
    @type  num: float
    @param num: A floating point number to write to the bitbuffer.'''
    pass
  
  def write_num(self, num):
    '''write_num(num)
    
    Writes a normal integer to a writable bitbuffer (bf_write).
    @note: nothing is actually written to a bitbuffer until necessary.
    @type  num: int
    @param num: An integer to write to the bitbuffer.'''
    pass
  
  def write_short(self, short):
    '''write_short(short)
    
    Writes a 16bit integer to a writable bitbuffer (bf_write).
    @note: nothing is actually written to a bitbuffer until necessary.
    @type  short: int
    @param short: A 16bit integer to write to the bitbuffer.'''
    pass
  
  def write_string(self, string):
    '''write_string(string)
    
    Writes a string to a writable bitbuffer (bf_write).
    @note: nothing is actually written to a bitbuffer until necessary.
    @type  string: str
    @param string: Text string to write to the bitbuffer.'''
    pass
  
  def write_vec_coord(self, vec):
    '''write_vec_coord(vec)
    
    Writes a 3D vector of coordinates to a writable bitbuffer (bf_write).
    @note: nothing is actually written to a bitbuffer until necessary.
    @type  vec: datatypes.Vector or list-like[3]
    @param vec: A Vector object or list-like object of 3 floats to write to the 
        bitbuffer.'''
    pass
  
  def write_vec_normal(self, vec):
    '''write_vec_normal(vec)
    
    Writes a 3D normal vector to a writable bitbuffer (bf_write).
    @note: nothing is actually written to a bitbuffer until necessary.
    @type  vec: datatypes.Vector or list-like[3]
    @param vec: A Vector object or list-like object of 3 floats to write to the 
        bitbuffer.'''
    pass
  
  def write_word(self, word):
    '''write_word(word)
    
    Writes a 16bit unsigned integer to a writable bitbuffer (bf_write).
    @note: nothing is actually written to a bitbuffer until necessary.
    @type  word: int
    @param word: A 16bit unsigned integer to write to the bitbuffer.'''
    pass
  

class BitBufRead:
  '''Interact with readable Source BitBuffers (bf_read).'''
  
  '''The number of bytes left in the bf_read.'''
  num_bytes_left = None
  def read_angle(self, numBits=8):
    '''read_angle([numBits=8]) -> float
    
    Reads a bit angle from the readable bitbuffer.
    @type  numBits: int
    @param numBits: Optional number of bits to use.
    @rtype: float
    @return: A bit angle'''
    pass
  
  def read_angles(self):
    '''read_angles() -> datatypes.Vector
    
    Reads a 3D angle vector from the readable bitbuffer.
    @rtype: datatypes.Vector
    @return: A datatypes.Vector object representing a 3D angle vector.'''
    pass
  
  def read_bool(self):
    '''read_bool() -> bool
    
    Reads a single bit from the readable bitbuffer.
    @rtype: bool
    @return: A single bit from the bitbuffer.'''
    pass
  
  def read_byte(self):
    '''read_byte() -> int
    
    Reads a byte from the readable bitbuffer.
    @rtype: int
    @return: The byte read from the bitbuffer. Only the first 8 bits are used.'''
    pass
  
  def read_char(self):
    '''read_char() -> str[1]
    
    Reads a character from the readable bitbuffer.
    @rtype: str[1]
    @return: A string of length 1 containing the character read.'''
    pass
  
  def read_coord(self):
    '''read_coord() -> float
    
    Reads a coordinate from the readable bitbuffer.
    @rtype: float
    @return: The coordinate read from the bitbuffer.'''
    pass
  
  def read_entity(self):
    '''read_entity() -> entity.Entity
    
    Reads an entity from the readable bitbuffer.
    @rtype: entity.Entity
    @return: An entity.Entity object representing an entity read from the bitbuffer.'''
    pass
  
  def read_float(self):
    '''read_float() -> float
    
    Reads a floating point number from the readable bitbuffer.
    @rtype: float
    @return: The floating point number read from the bitbuffer.'''
    pass
  
  def read_num(self):
    '''read_num() -> int
    
    Reads a normal integer from the readable bitbuffer.
    @rtype: int
    @return: The 32-bit integer value read from the bitbuffer.'''
    pass
  
  def read_short(self):
    '''read_short() -> int
    
    Reads a 16-bit integer from the readable bitbuffer.
    @rtype: int
    @return: The 16-bit integer value read from the bitbuffer.'''
    pass
  
  def read_string(self, lines=False):
    '''read_string([lines=False]) -> str
    
    Reads a string from the readable bitbuffer.
    @type  lines: bool
    @param lines: If True the buffer will be read until it reaches a newline or
        null terminator.@rtype: str
    @return: The string value read from the bitbuffer.'''
    pass
  
  def read_vec_coord(self):
    '''read_vec_coord() -> datatypes.Vector
    
    Reads a 3D vector of coordinates from the readable bitbuffer.
    @rtype: datatypes.Vector
    @return: A datatypes.Vector object representing a 3D vector of coordinates.'''
    pass
  
  def read_vec_normal(self):
    '''read_vec_coord() -> datatypes.Vector
    
    Reads a 3D normal vector from the readable bitbuffer.
    @rtype: datatypes.Vector
    @return: A datatypes.Vector object representing a 3D normal vector.'''
    pass
  
  def read_word(self):
    '''read_short() -> int
    
    Reads a 16-bit unsigned integer from the readable bitbuffer.
    @rtype: int
    @return: The 16-bit unsigned integer value read from the bitbuffer.'''
    pass
  

