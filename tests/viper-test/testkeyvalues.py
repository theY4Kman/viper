import unittest
from sourcemod import keyvalues
import sys
from binascii import crc32

class KeyValuesModuleTestCase(unittest.TestCase):
  def test_keyvalues_functions_exist(self):
    '''sourcemod.keyvalues functions exist'''
    self.assert_(hasattr(keyvalues, 'keyvalues_from_file') and
        callable(getattr(keyvalues, 'keyvalues_from_file')),
        'keyvalues.keyvalues_from_file does not exist or is not callable')
  
  def test_keyvalues_types_exist(self):
    '''sourcemod.keyvalues KeyValues type exists'''
    self.assert_(hasattr(keyvalues, 'KeyValues') and type(keyvalues.KeyValues) is type,
        'keyvalues.KeyValues does not exist or is not a base type')


class KeyValuesTestCase(unittest.TestCase):
  def test_example_file_is_unchanged(self):
    '''KeyValues file has been unchanged'''
    fp = open(sys.path[0]+'/keyvalues_example.cfg', 'r')
    contents = fp.read()
    fp.close()
    self.assertEqual(crc32(contents), 0x1d6d0eb2, 'the KeyValues test file, '
        '"keyvalues_example.cfg", has been changed, rendering the test useless.')
  
  def test_from_file(self):
    '''KeyValues creation from file'''
    kv = keyvalues.keyvalues_from_file(sys.path[0]+'/keyvalues_example.cfg')
    self.assertTrue(kv, 'could not create KeyValues from file')
  
  def test_from_file_validity(self):
    '''KeyValues object matches file'''
    kv = keyvalues.keyvalues_from_file(sys.path[0]+'/keyvalues_example.cfg')
    self.assertTrue(kv, 'could not create KeyValues from file')
    
    self.assertEqual(kv.name, 'root_section_name', 'the root section name of '
        'the KeyValues object differs from the file (expected "root_section_name",'
        ' found "%s")' % kv.name)
    kv.uses_escape_sequences = True
    # TODO: create kv object from string and compare it to file
    # TODO: write comparison functions for KeyValues object in C++
