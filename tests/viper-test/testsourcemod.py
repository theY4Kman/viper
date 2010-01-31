import sys
import unittest
import sourcemod

class SourceModModuleTestCase(unittest.TestCase):
  def test_sourcemod_functions_exist(self):
    '''sourcemod functions exist'''
    for name in ['get_sourcemod_path', 'get_game_path']:
      self.assert_(hasattr(sourcemod, name) and
          callable(getattr(sourcemod, name)), 'sourcemod.%s does not exist or is '
          'not callable' % name)


class KeyValuesTestCase(unittest.TestCase):
  def test_game_path(self):
    '''Game path is in sys.path'''
    self.assert_(sourcemod.get_game_path().replace('\\', '/') in sys.path[0],
        '%s not found in %s' % (sourcemod.get_game_path().replace('\\', '/'), sys.path[0]))
  
  def test_sourcemod_path(self):
    '''SourceMod path in sys.path'''
    self.assert_(sourcemod.get_sourcemod_path().replace('\\', '/') in sys.path[0],
        '%s not found in %s' % (sourcemod.get_sourcemod_path().replace('\\', '/'), sys.path[0]))
    
    end = 'thoushaltalwayskill'
    self.assert_(sourcemod.get_sourcemod_path(end).replace('\\', '/').endswith(end),
        '"%s" does not end with "%s"' % (sourcemod.get_sourcemod_path(end).replace('\\', '/'), end))
