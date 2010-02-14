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
  def setUp(self):
    self.kv_dict = {
      'key1': 'value1',
      'escape': r'\\\n',
      'subsection1':
      {
        'subkey1': 'subvalue1',
        'subkey2': 2,
        'subkey3': 4.7123,
      }
    }
    self.kv = keyvalues.KeyValues('root_section_name', self.kv_dict)
  
  def recursive_dict_check(self, d, kv):
    for key,value in d.items():
      self.assertTrue(key in kv, 'KV key "%s" does not exist' % key)
      
      if type(value) is dict:
        self.recursive_dict_check(value, kv[key])
      else:
        if type(value) == type(kv[key]) and type(value) is float:
          # The value stored in the dict retains the same amount of decimal
          # digits, but the value stored in the KeyValues object loses this
          # information and becomes 4.71229982376, instead of 4.7123, so we
          # must round before checking equality.
          self.assertAlmostEqual(value, kv[key], 4, 'KV key "%s" differs with'
              ' dict (expected "%s", found "%s")' % (key, value, kv[key]))
        else:
          self.assertEqual(value, kv[key], 'KV key "%s" differs with dict '
              '(expected "%s", found "%s")' % (key, value, kv[key]))
  
  def test_validity(self):
    '''KeyValues object was created correctly'''
    self.assertTrue(self.kv, 'could not create KeyValues')
    
    self.assertEqual(self.kv.name, 'root_section_name', 'the root section name of '
        'the KeyValues object differs from the file (expected "root_section_name",'
        ' found "%s")' % self.kv.name)
    self.recursive_dict_check(self.kv_dict, self.kv)
  
  def test_iter_funcs_exist(self):
    '''KeyValues iter functions exist'''
    self.assertTrue(self.kv.iteritems(), 'iteritems returned None')
    self.assertTrue(self.kv.iterkeys(), 'iterkeys returned None')
    self.assertTrue(self.kv.itervalues(), 'itervalues returned None')
  
  def test_itervalues(self):
    '''KeyValues itervalues iterates correct information'''
    dvalues = self.kv_dict.values()
    for value in self.kv.itervalues():
      if type(value) is keyvalues.KeyValues:
        # TODO: test subsections
        self.assertTrue(True, 'KeyValues subsection not found in dict')
      else:
        self.assert_(value in dvalues, 'value not found in dict (looking for '
            '"%s")' % str(value))
  
  def test_iterkeys(self):
    '''KeyValues iterkeys iterates correct information'''
    keys = []
    for key in self.kv.iterkeys():
      keys.append(key)
    for key in self.kv_dict.iterkeys():
      self.assert_(key in keys, 'KeyValues iterkeys does not include all info '
          'from the dict (looking for "%s")' % key)
  
  def test_iteritems(self):
    '''KeyValues iteritems iterates correct information'''
    dvalues = self.kv_dict.values()
    keys = []
    for key,value in self.kv.iteritems():
      keys.append(key)
      if type(value) is keyvalues.KeyValues:
        # TODO: test subsections
        self.assertTrue(True, 'KeyValues subsection not found in dict')
      else:
        self.assert_(value in dvalues, 'value not found in dict (looking for '
            '"%s")' % str(value))
    for key in self.kv_dict.iterkeys():
      self.assert_(key in keys, 'KeyValues iteritems does not include all keys'
          ' from the dict (looking for "%s")' % key)
  
  def test_has_key(self):
    '''KeyValues has_key finds key'''
    self.assertTrue(self.kv.has_key('key1'), 'has_key could not find the key')


class KeyValuesFileTestCase(KeyValuesTestCase):
  def setUp(self):
    # Save old KV object
    super(KeyValuesFileTestCase, self).setUp()
    self._kv = self.kv
    
    self.kv = keyvalues.keyvalues_from_file(sys.path[0]+'/keyvalues_example.cfg')
  
  def test_example_file_is_unchanged(self):
    '''KeyValues file has been unchanged'''
    fp = open(sys.path[0]+'/keyvalues_example.cfg', 'r')
    contents = fp.read()
    fp.close()
    self.assertEqual(crc32(contents), 0x1d6d0eb2, 'the KeyValues test file, '
        '"keyvalues_example.cfg", has been changed, rendering the test useless.')
