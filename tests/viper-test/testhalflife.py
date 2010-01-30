import unittest
import sys
from sourcemod import *

class HalfLifeTestCase(unittest.TestCase):
  def testcurrentmap(self):
    '''Current map has name'''
    self.assertTrue(halflife.get_current_map(), 'invalid map name')
  
  def testenginetime(self):
    '''Engine time is non-zero'''
    self.assertTrue(halflife.get_engine_time(), 'invalid engine time')
  
  def testgametime(self):
    '''Game time is non-zero'''
    self.assertTrue(halflife.get_game_time(), 'invalid game time')
  
  def testgamedescription(self):
    '''Game description valid'''
    self.assertTrue(halflife.get_game_description(), 'game description invalid')
    self.assertTrue(halflife.get_game_description(original=True), 'original game description invalid')
  
  def testgamefolder(self):
    '''Valid game folder (is in sys.path)'''
    self.assert_(halflife.get_game_folder_name() in sys.path[0])
