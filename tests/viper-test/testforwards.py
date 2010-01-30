import unittest
from sourcemod import *

class ForwardModuleTestCase(unittest.TestCase):
  def testexectypes(self):
    '''sourcemod.forwards exec types exist'''
    for et in ['Ignore', 'Single', 'Event', 'Hook', 'LowEvent']:
      self.assertTrue(hasattr(forwards, 'ET_'+et))
  
  def testforwardfunctions(self):
    '''sourcemod.forwards functions exist'''
    self.assert_(hasattr(forwards, 'create') and
                 callable(forwards.create), 'forwards.create does not exist')
    self.assert_(hasattr(forwards, 'register') and
                 callable(forwards.register), 'forwards.register does not exist')


class AnonymousForwardTestCase(unittest.TestCase):
  def anon_fwd_process(self, ret, func):
    self.processed = ret
    return ret
  
  def setUp(self):
    self.fwd = forwards.create('', self.anon_fwd_process, forwards.ET_Hook, int)
    self.processed = None
  
  def testname(self):
    '''Anonymous forward has blank name'''
    self.assertEqual(self.fwd.name, '', 'anonymous forward name should be blank,'
        ' but is "%s"' % self.fwd.name)
  
  def testaddcallback(self):
    '''Adding/removing a callback to an anonymous forward'''
    def callback(arg1): pass
    self.fwd.add_function(callback)
    self.assertEqual(self.fwd.get_function_count(), 1, 'wrong amount of callbacks returned')
    self.fwd.remove_function(callback)
    self.assertEqual(self.fwd.get_function_count(), 0, 'wrong amount of callbacks returned')
  
  def testlen(self):
    '''len(forward) == forward.get_function_count()'''
    def callback(arg1): pass
    self.fwd.add_function(callback)
    self.assertEqual(self.fwd.get_function_count(), len(self.fwd),
        'len does not match get_function_count')
    self.fwd.remove_function(callback)
    self.assertEqual(self.fwd.get_function_count(), len(self.fwd),
        'len does not match get_function_count')
  
  def testcontains(self):
    '''Callback is in forward (using `x in y` syntax)'''
    def callback(arg1): pass
    self.fwd.add_function(callback)
    self.assert_(callback in self.fwd, 'callback not contained in forward')
    self.fwd.remove_function(callback)
  
  def testitem(self):
    '''Callback is in forward (using `y[0] == x` syntax)'''
    def callback(arg1): pass
    self.fwd.add_function(callback)
    self.assertEqual(self.fwd[0], callback, 'callback not contained in forward')
    self.fwd.remove_function(callback)
  
  def testreturnvalue(self):
    '''Forward with ET_Hook returns highest number'''
    def callback1(arg1): return 5
    def callback2(arg1): return 2
    def callback3(arg1): return 1
    def callback4(arg1): return -8
    def callback5(arg1): return "sawce"
    
    for x in xrange(1,5+1):
      self.fwd.add_function(locals()['callback'+str(x)])
    
    ret = self.fwd.fire(0)
    self.assertEqual(ret, Plugin_Stop, 'incorrect value returned (got %d, expected Plugin_Stop)' % ret)
    
    for x in xrange(1,5+1):
      self.fwd.remove_function(locals()['callback'+str(x)])
    self.assertEqual(self.fwd.get_function_count(), 0, 'wrong amount of callbacks returned')
  
  def testprocessor(self):
    '''Forward with ET_Hook returns highest Plugin_XXXX value'''
    def callback(arg1): return "sawce"
    
    self.fwd.add_function(callback)
    
    self.fwd.fire(0)
    self.assertEqual(self.processed, "sawce", 'callback processor not called '
        '(looking for value "sawce", got %s)' % repr(self.processed))
    
    self.fwd.remove_function(callback)
    self.assertEqual(self.fwd.get_function_count(), 0, 'wrong amount of callbacks returned')
  
  def test_fire_with_wrong_types(self):
    '''Forward doesn't allow firing with incorrect types'''
    def callback(arg1): return "sawce"
    
    self.fwd.add_function(callback)
    self.assertRaises(TypeError, self.fwd.fire, "incorrect argument type")
    self.fwd.remove_function(callback)


class GlobalForwardTestCase(unittest.TestCase):
  def setUp(self):
    self.fwd = forwards.create('backward', None, forwards.ET_LowEvent)
  
  def test_register(self):
    '''Global forward registration'''
    def callback(): return Plugin_Stop 
    
    forwards.register('backward', callback)
    self.assertEqual(self.fwd.get_function_count(), 1, 'wrong amount of callbacks registered')
    
    self.assertEqual(self.fwd.fire(), Plugin_Stop, 'callback never called, assumed unregistered')
    
    self.fwd.remove_function(callback)
    self.assertEqual(self.fwd.get_function_count(), 0, 'wrong amount of callbacks registered')
