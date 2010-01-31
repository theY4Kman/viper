import unittest
from sourcemod import events, Plugin_Continue

class EventsModuleTestCase(unittest.TestCase):
  def test_events_functions_exist(self):
    '''sourcemod.events functions exist'''
    for name in ['create', 'hook', 'unhook']:
      self.assert_(hasattr(events, name) and
          callable(getattr(events, name)), 'events.%s does not exist or is '
          'not callable' % name)
  
  def test_event_hook_mode_defines_exist(self):
    '''sourcemod.events EventHookModes exist'''
    for name in ['Pre', 'Post', 'PostNoCopy']:
      self.assertTrue(hasattr(events, 'EventHookMode_'+name), 'events.%s'
          'does not exist' % ('EventHookMode_'+name,))
  
  def test_events_types_exist(self):
    '''sourcemod.events Event type exists'''
    self.assert_(hasattr(events, 'Event') and type(events.Event) is type,
        'events.Event does not exist or is not a base type')


class EventsTestCase(unittest.TestCase):
  def test_event_create(self):
    '''Event creation'''
    evt = events.create('round_start')
    self.assertTrue(evt, 'Event creation unsuccessful (maybe the mod doesn\'t '
        'support "round_start"?)')
    evt.cancel()
  
  def test_event_create(self):
    '''Invalid Event creation not allowed'''
    evt = events.create('a non-existent event name')
    self.assertFalse(evt, 'Event creation successful, but it shouldn\'t be')
  
  def test_event_object_attributes_exist(self):
    '''Event object attributes exist'''
    evt = events.create('round_start')
    for name in ['name', 'dont_broadcast']:
      self.assertTrue(hasattr(evt, name), 'events.Event.%s does not exist' % name)
    evt.cancel()
  
  def test_event_object_methods_exist(self):
    '''Event object methods exist'''
    evt = events.create('round_start')
    for name in ['cancel', 'fire', 'has_field']+[p+'et_'+f for p in 'sg'
                 for f in ['bool','float','int','string']]:
      self.assertTrue(hasattr(evt, name), 'events.Event.%s does not exist' % name)
    evt.cancel()
  
  def test_event_name(self):
    '''Event.name matches creation'''
    name = 'round_start'
    evt = events.create(name)
    self.assertEqual(evt.name, name, 'name created with and name attribute differ'
        ' (expected "%s", found "%s")' % (name, evt.name))
  
  def test_event_name_change(self):
    '''Event.name is read-only'''
    name = 'round_start'
    evt = events.create(name)
    self.assertRaises(AttributeError, setattr, evt, 'name', 'fuzzbuggler')
    evt.cancel()
  
  def test_event_fire(self):
    '''Event field values exist and match'''
    self.values = False
    def callback(evt, name):
      for name in ['timelimit', 'fraglimit', 'objective']:
        self.assertTrue(evt.has_field(name), 'round_start event should have '
            'field "%s"' % name)
      self.values = (evt.get_int('timelimit'), evt.get_int('fraglimit'), 
          evt.get_string('objective'))
      return Plugin_Continue
    
    name = 'round_start'
    timelimit = 67
    fraglimit = 42
    objective = 'PANTS ON THE GROUND'
    
    events.hook(name, callback)
    
    evt = events.create(name)
    evt.set_int('timelimit', timelimit)
    evt.set_int('fraglimit', fraglimit)
    evt.set_string('objective', objective)
    evt.fire()
    
    self.assertTrue(self.values, 'event callback never called')
    self.assertEqual(self.values[0], timelimit, 'timelimit set and timelimit '
        'retrieved differ (expected %d, found %d)' % (timelimit, self.values[0]))
    self.assertEqual(self.values[1], fraglimit, 'fraglimit set and fraglimit '
        'retrieved differ (expected %d, found %d)' % (fraglimit, self.values[1]))
    self.assertEqual(self.values[2], objective, 'objective set and objective '
        'retrieved differ (expected %s, found %s)' % (objective, self.values[2]))
