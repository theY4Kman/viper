import unittest
import random
from sourcemod import bitbuf

class BitBufModuleTestCase(unittest.TestCase):
  def test_bitbuf_type_exist(self):
    '''sourcemod.bitbuf.BitBuf type exists'''
    self.assertTrue(hasattr(bitbuf, 'BitBuf'))


class BitBufTestCase(unittest.TestCase):
  def test_functions_exist(self):
    '''sourcemod.bitbuf.BitBuf functions exist'''
    for name in ['write_angle', 'write_bool', 'write_byte', 'write_coord',
                 'write_char', 'write_entity', 'write_float', 'write_num',
                 'write_short', 'write_string', 'write_vec_coord',
                 'write_vec_normal', 'write_word']:
      self.assertTrue(hasattr(bitbuf.BitBuf, name), '"%s" does not exist in the'
          ' bitbuf.BitBuf type.' % name)
  

