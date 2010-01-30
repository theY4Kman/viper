import unittest
from sourcemod import *
  
# :TODO: create a netprops array test

class EntityModuleTestCase(unittest.TestCase):
  def setUp(self):
    self.assertTrue(entity, 'entity module not found')
  
  def testcreateedict(self):
    '''entity.create_edict() is not None'''
    self.failUnless(entity.create_edict(), 'could not create a new edict')
  
  def testentitycount(self):
    '''entity.get_entity_count() > 0'''
    self.failUnless(entity.get_entity_count(), 'invalid entity count')
  
  def testmaxents(self):
    '''entity.get_max_entities() > 0'''
    self.failUnless(entity.get_max_entities(), 'invalid max entity count')


class WorldEntityTestCase(unittest.TestCase):
  def setUp(self):
    self.assertTrue(entity, 'entity module not found')
    
    self.world = entity.Entity(0)
    self.assertTrue(self.world)
  
  def testnetclass(self):
    '''World netclass == "CWorld"'''
    self.assertEqual(self.world.netclass, 'CWorld', 'World netclass incorrect')
  
  def testclassname(self):
    '''World classname == "worldspawn"'''
    self.assertEqual(self.world.classname, 'worldspawn', 'World classname incorrect')
  
  def testnetworkable(self):
    '''World is networkable'''
    self.assertTrue(self.world.is_networkable(), 'World should be networkable')
  
  def testvalidity(self):
    '''World is valid entity'''
    self.assertTrue(self.world.is_valid(), 'World should be valid')
  
  def testdatamaps(self):
    '''World has datamaps'''
    self.assertTrue(self.world.datamaps, 'World has no datamaps')
  
  def testdatamapsvalid(self):
    '''World datamaps["m_iClassname"] == "worldspawn"'''
    self.assertEqual(self.world.datamaps['m_iClassname'], 'worldspawn',
      'm_iClassname datamap has incorrect value')
  
  def testdatamapsinvalid(self):
    '''World datamaps["invalid datamap name"] raises exception'''
    def invaliddatamap():
      self.world.datamaps['invalid datamap name']
    self.assertRaises(ViperError, invaliddatamap)
  
  def testsendprops(self):
    '''World has sendprops'''
    self.assertTrue(self.world.sendprops, 'World has no sendprops')
  
  def testsendpropsvalid(self):
    '''World sendprops["m_nModelIndex"] == 1'''
    self.assertEqual(self.world.sendprops['m_nModelIndex'], 1,
      'm_nModelIndex sendprop has incorrect value')
  
  def testsendpropsinvalid(self):
    '''World sendprops["invalid sendprop name"] raises exception'''
    def invalidsendprop():
      self.world.sendprops['invalid sendprop name']
    self.assertRaises(ViperError, invalidsendprop)
