import unittest
import libtest
import sourcemod as sm
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
    for name in ['cancel', 'fire', 'is_empty']:
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
  
  # Holds the game_events for different mods. The field values include both
  # the value and the type they should be returned as when using evt[field]
  game_events = {
    'tf': ('item_found',
      {
        'player': (0, int),
        'quality': (20, int),
        'item': ('poopsicles', str),
        'method': (0, int),
        'propername': (True, bool)
      }),
    'cstrike': ('round_start',
      {
        'timelimit': (69, int),
        'fraglimit': (42, int),
        'objective': ('PANTS ON THE GROUND', str),
      })
  }
  
  def test_event_fire(self):
    '''Event field values exist and match'''
    self.values = False
    
    event = self.game_events.get(sm.halflife.get_game_folder_name(), None)
    if event is None:
      print 'Unable to locate a test for your mod, using cstrike...'
      event = self.game_events['cstrike']
    
    evt_name = event[0]
    fields = event[1]
    
    def callback(evt, evt_name):
      self.values = True
      
      for field in fields.iterkeys():
        # Check each field exists
        self.assertTrue(evt.has_field(field), '%s event should have field "%s"'
            % (evt_name, field))
        if not evt.has_field(field):
          continue
        
        # Now check that the fields match what was written to them
        self.assertEqual(evt[field], fields[field][0], 'Field "%s" set and '
            'field %s retrieved differ (expected "%s", found "%s")' %
            (field, field, fields[field][0], evt[field]))
        
        # Now check that the field types match modevents.res
        self.assertEqual(type(evt[field]), fields[field][1], 'Field "%s" is of'
            ' incorrect type "%s" (expected "%s", found "%s")' %
            (field, str(type(evt[field])), str(fields[field][1]),
            str(type(evt[field]))))
      
      return Plugin_Continue
    
    events.hook(evt_name, callback)
    
    evt = events.create(evt_name)
    for field,value in fields.iteritems():
      evt[field] = value[0]
    evt.fire()
    
    self.assertTrue(self.values, 'event callback never called. This is '
        'probably due to events being handled on the next gameframe, and '
        'thus isn\'t an issue.')
      
