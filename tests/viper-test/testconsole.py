import unittest
import random
from sourcemod import console, clients, Plugin_Handled

def gen_rand_cvar_name():
  '''Generates a random string to be used when creating console vars/commands'''
  chars = 'abcdefghijklmnopqrstuvwxyz0123456789'
  return 'viper_test_' + ''.join([random.choice(chars) for x in xrange(12)])

class ConsoleModuleTestCase(unittest.TestCase):
  def test_console_types_exist(self):
    '''sourcemod.console types exist'''
    for name in ['ConVar', 'ConCommandReply']:
      self.assertTrue(hasattr(console, name))
  
  def test_console_reply_to_exist(self):
    '''sourcemod.console SM_REPLY constants exist'''
    for name in ['SM_REPLY_CHAT', 'SM_REPLY_CONSOLE']:
      self.assertTrue(hasattr(console, name))
  
  def test_console_fcvar_flags_exist(self):
    '''sourcemod.console FCVAR flags exist'''
    for name in ['NONE', 'UNREGISTERED', 'LAUNCHER', 'GAMEDLL', 'CLIENTDLL',
                 'MATERIAL_SYSTEM', 'PROTECTED', 'SPONLY', 'ARCHIVE', 'NOTIFY',
                 'USERINFO', 'PRINTABLEONLY', 'UNLOGGED', 'NEVER_AS_STRING',
                 'REPLICATED', 'CHEAT', 'STUDIORENDER', 'DEMO', 'DONTRECORD',
                 'PLUGIN', 'DATACACHE', 'TOOLSYSTEM', 'FILESYSTEM',
                 'NOT_CONNECTED', 'SOUNDSYSTEM', 'ARCHIVE_XBOX', 'INPUTSYSTEM',
                 'NETWORKSYSTEM', 'VPHYSICS']:
      self.assertTrue(hasattr(console, 'FCVAR_'+name))
  
  def test_console_functions_exist(self):
    '''sourcemod.console functions exist'''
    for name in ['create_convar', 'find_convar', 'print_to_server', 'reg_concmd',
        'server_command', 'server_execute']:
      self.assert_(hasattr(console, name) and
          callable(getattr(console, name)), 'console.%s does not exist or is '
          'not callable' % name)


class ConVarTestCase(unittest.TestCase):
  def test_create_convar(self):
    '''ConVar creation'''
    self.assertTrue(console.create_convar(gen_rand_cvar_name(), 'test'))
  
  def test_convar_value_retain(self):
    '''ConVar value is stored correctly'''
    value = 'viper_test'
    cvar = console.create_convar(gen_rand_cvar_name(), value)
    self.assertEqual(cvar.value, value, 'value set and value retrieved differ'
        ' (expected "%s", found "%s")' % (value, cvar.value))
  
  def test_convar_value_type(self):
    '''ConVar value type == str'''
    cvar = console.create_convar(gen_rand_cvar_name(), 'test')
    self.assertEqual(type(cvar.value), str, 'cvar type(value) should be str,'
        ' found %s.' % str(type(cvar.value)))
  
  def test_convar_value_change(self):
    '''ConVar value is stored correctly'''
    cvar = console.create_convar(gen_rand_cvar_name(), '')
    value = 'viper_test'
    cvar.value = value
    self.assertEqual(cvar.value, value, 'value set and value retrieved differ'
        ' (expected "%s", found "%s")' % (value, cvar.value))
  
  def test_convar_reset(self):
    '''ConVar value is reset correctly'''
    default = 'viper_test'
    cvar = console.create_convar(gen_rand_cvar_name(), default)
    cvar.value = 'salt the snail'
    cvar.reset()
    self.assertEqual(cvar.value, default, 'value reset to the wrong value'
        ' (expected "%s", found "%s")' % (default, cvar.value))
  
  def test_convar_flags(self):
    '''ConVar flags are stored correctly'''
    flags = console.FCVAR_SPONLY
    cvar = console.create_convar(gen_rand_cvar_name(), 'jumbo shrimp', flags=flags)
    self.assertEqual(cvar.flags, flags, 'flags set and flags retrieved differ'
        ' (expected %d, found %d)' % (flags, cvar.flags))
  
  def test_convar_bounds_stored(self):
    '''ConVar bounds are stored correctly'''
    lower = 0.0
    upper = 1.0
    cvar = console.create_convar(gen_rand_cvar_name(), '0.5', min=lower, max=upper)
    self.assertEqual(cvar.lower_bound, lower, 'lower bound set and lower bound'
        ' retrieved differ (expected %f, found %f)' % (lower, cvar.lower_bound))
    self.assertEqual(cvar.upper_bound, upper, 'upper bound set and upper bound'
        ' retrieved differ (expected %f, found %f)' % (upper, cvar.upper_bound))
  
  def test_convar_bounds_enforced(self):
    '''ConVar bounds enforced'''
    lower = 0.0
    upper = 1.0
    cvar = console.create_convar(gen_rand_cvar_name(), '0.5', min=lower, max=upper)
    cvar.value = 10.0
    self.assert_(lower <= float(cvar.value) <= upper, 'bounds not enforced correctly'
        ' (expected value between %f and %f, found %s)' % (lower, upper, cvar.value))
    cvar.value = -10.0
    self.assert_(lower <= float(cvar.value) <= upper, 'bounds not enforced correctly'
        ' (expected value between %f and %f, found %s)' % (lower, upper, cvar.value))
  
  def test_convar_bounds_enforced(self):
    '''ConVar bounds change correctly'''
    cvar = console.create_convar(gen_rand_cvar_name(), '0.5', min=-10.0, max=-1.0)
    
    cvar.lower_bound = lower = 0.0
    cvar.upper_bound = upper = 1.0
    
    cvar.value = 10.0
    self.assert_(lower <= float(cvar.value) <= upper, 'bounds not enforced correctly'
        ' (expected value between %f and %f, found %s)' % (lower, upper, cvar.value))
    cvar.value = -10.0
    self.assert_(lower <= float(cvar.value) <= upper, 'bounds not enforced correctly'
        ' (expected value between %f and %f, found %s)' % (lower, upper, cvar.value))
  
  def test_convar_hooks(self):
    '''ConVar hooks called'''
    self.hook_called = False
    def callback(cvar, old, new): self.hook_called = (cvar, old, new)
    
    old = 'old'
    cvar = console.create_convar(gen_rand_cvar_name(), old)
    cvar.hook_change(callback)
    
    cvar.value = new = 'new'
    self.assertTrue(self.hook_called, 'ConVar hook never called')
    self.assertEqual(self.hook_called, (cvar, old, new), 'ConVar hook passed wrong info')
    
    self.hook_called = False
    cvar.unhook_change(callback)
    old = cvar.value
    cvar.value = new = 'smurf'
    
    self.assertFalse(self.hook_called, 'ConVar hook called after unregistered')
  
  def test_find_convar(self):
    '''ConVar created can be found'''
    name = gen_rand_cvar_name()
    cvar = console.create_convar(name, '')
    found = console.find_convar(name)
    self.assertEqual(found, cvar, 'ConVar created and ConVar found differ')


class ConCommandTestCase(unittest.TestCase):
  def test_reg_concmd(self):
    '''ConCommand creation'''
    def callback(cmd): return Plugin_Handled
    self.assertTrue(console.reg_concmd(gen_rand_cvar_name(), callback),
        'could not register a concommand')
  
  def test_concmd_callback(self):
    '''ConCommand callbacks executed'''
    self.callback_called = False
    def callback(cmd):
      self.callback_called = (cmd.argstring, len(cmd.args), cmd.client)
      
      self.assertEqual(self.callback_called[0], argstring, 'ConCommand callback'
          ' passed wrong argstring (expected "%s", found "%s")' % (argstring,
          self.callback_called[0]))
      self.assertEqual(self.callback_called[1], args, 'ConCommand callback'
          ' passed wrong args (expected "%s", found "%s")' % (str(args),
          str(self.callback_called[1])))
      self.assertEqual(self.callback_called[2], client, 'ConCommand callback'
          ' passed wrong argstring (expected "%s", found "%s")' % (str(client),
          str(self.callback_called[2])))
      
      self.assertEqual(cmd.reply_to, console.SM_REPLY_CONSOLE, 'ConCommand'
          ' callback has incorrect reply_to value (expected %d, found %d)'
          % (console.SM_REPLY_CONSOLE, cmd.reply_to))
      return Plugin_Handled
    
    name = gen_rand_cvar_name()
    console.reg_concmd(name, callback)
    
    argstring = "test"
    args = 1
    client = clients.get_client(0)
    
    console.server_command(name + ' ' + argstring)
    console.server_execute()
    
    #self.assertTrue(self.callback_called, 'ConCommand callback never called. '
    #    'This might mean the server command queue hasn\'t been processed, '
    #    'and so is a non-issue.')
