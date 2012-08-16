import unittest
from sourcemod import *

class ClientsModuleTestCase(unittest.TestCase):
  def test_clients_functions_exist(self):
    '''sourcemod.clients functions exist'''
    for name in ['create_fake_client', 'get_client', 'get_client_count',
                 'get_client_from_serial', 'get_client_from_userid',
                 'get_max_clients']:
      self.assert_(hasattr(clients, name) and
          callable(getattr(clients, name)), 'clients.%s does not exist or is '
          'not callable' % name)
  
  def test_clients_banflag_defines_exist(self):
    '''sourcemod.clients BANFLAG defines exist'''
    for name in ['AUTO', 'IP', 'AUTHID', 'NOKICK']:
      self.assertTrue(hasattr(clients, 'BANFLAG_'+name), 'clients.%s'
          'does not exist' % ('BANFLAG_'+name,))
  
  def test_clients_types_exist(self):
    '''sourcemod.clients Client type exists'''
    self.assert_(hasattr(clients, 'Client') and type(clients.Client) is type,
        'clients.Client does not exist or is not a base type')
  
  def test_clients_max(self):
    '''sourcemod.clients.get_client respects client index limits'''
    def low_limit():
      return clients.get_client(-1)
    def high_limit():
      return clients.get_client(clients.get_max_clients() + 1)
    
    self.assertRaises(ViperError, low_limit)
    self.assertRaises(ViperError, high_limit)


class ClientsTypeTestCase(unittest.TestCase):
  def test_clients_index(self):
    '''sourcemod.clients.Client index correct for server console'''
    server = clients.get_client(0)
    self.assertEqual(server.index, 0, 'Server console client index should be'
        ' 0, but is %d' % server.index)
  
  def test_clients_attributes(self):
    '''sourcemod.clients.Client attributes exist'''
    for name in ['abs_angles', 'abs_origin', 'alive', 'armor', 'avg_choke',
        'avg_choke_in', 'avg_choke_out', 'avg_data', 'avg_data_in',
        'avg_data_out', 'avg_latency', 'avg_latency_in', 'avg_latency_out',
        'avg_loss', 'avg_loss_in', 'avg_loss_out', 'avg_packets',
        'avg_packets_in', 'avg_packets_out', 'data_rate', 'deaths', 'entity',
        'fake', 'frags', 'health', 'ip', 'lang_id', 'latency', 'latency_in',
        'latency_out', 'maxs', 'mins', 'model', 'name', 'serial', 'steamid',
        'team', 'time_connected', 'userid', 'weapon', 'index']:
      self.assertTrue(hasattr(clients.Client, name))
  
  def test_clients_functions_exist(self):
    '''sourcemod.clients.Client functions exist'''
    for name in ['fake_command', 'is_timing_out', 'kick',
                 'notify_post_admin_check', 'print_center', 'print_console',
                 'print_chat', 'print_hint', 'set_fake_client_convar',
                 'show_vgui_panel', 'is_connected']:
      self.assert_(hasattr(clients.Client, name) and
          callable(getattr(clients.Client, name)), 'clients.Client.%s does not '
          'exist or is not callable' % name)
