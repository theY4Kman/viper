import unittest
from sourcemod import clients

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


class ClientsTestCase(unittest.TestCase):
  pass
