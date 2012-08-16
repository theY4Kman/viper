import sys
import unittest
import sourcemod as sm

engine = sm.halflife.guess_sdk_version()
game = sm.halflife.get_game_folder_name()

def is_tf2():
  return game == 'tf'

def is_css():
  return game == 'cstrike'