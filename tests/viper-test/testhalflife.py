import unittest
import sys
from sourcemod import *

class HalfLifeTestCase(unittest.TestCase):
  def test_current_map(self):
    '''Current map has name'''
    self.assertTrue(halflife.get_current_map(), 'invalid map name')
  
  def test_current_map_is_valid(self):
    '''Current map is valid'''
    self.assertTrue(halflife.is_map_valid(halflife.get_current_map()),
        'current map is invalid, but this is impossible')
  
  def test_engine_time(self):
    '''Engine time is non-zero'''
    self.assertTrue(halflife.get_engine_time(), 'invalid engine time')
  
  def test_game_time(self):
    '''Game time is non-zero'''
    self.assertTrue(halflife.get_game_time(), 'invalid game time')
  
  def test_game_description(self):
    '''Game description valid'''
    self.assertTrue(halflife.get_game_description(), 'game description invalid')
    self.assertTrue(halflife.get_game_description(original=True), 'original game description invalid')
  
  def test_game_folder(self):
    '''Valid game folder (is in sys.path)'''
    self.assert_(halflife.get_game_folder_name() in sys.path[0])
  
  def test_is_dedicated_server(self):
    '''halflife.is_dedicated_server() exists'''
    self.assertTrue(hasattr(halflife, 'is_dedicated_server') and
                    callable(halflife.is_dedicated_server))
  
  def test_guess_sdk_version(self):
    '''Guess SDK version returns valid constant'''
    self.assert_(halflife.guess_sdk_version() in [
      halflife.SOURCE_SDK_UNKNOWN,
      halflife.SOURCE_SDK_ORIGINAL,
      halflife.SOURCE_SDK_DARKMESSIAH,
      halflife.SOURCE_SDK_EPISODE1,
      halflife.SOURCE_SDK_EPISODE2,
      halflife.SOURCE_SDK_LEFT4DEAD
    ], 'garbage SDK version guess')
  
  # :TODO: precaching tests
