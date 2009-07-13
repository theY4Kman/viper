==============================================
:mod:`sourcemod` -- The standard Viper library
==============================================

..  module:: sourcemod
..  moduleauthor:: Zach "theY4Kman" <they4kman@gmail.com>

This is the standard library provided by Viper. It is all written in C, and should be considered the fastest way to interact with the Source Engine.

..  exception:: ViperException
    
    Used for all Source-related errors. For example, this would be used to indicate an invalid entity, but not an object of the wrong type given to a function.

..  class:: server_out
    
    Writes to the server console. An instance of this class is used to overwrite `sys.stdout` in order to provide `print` redirection.

..  method:: write(msg)
    
    Prints *msg* to the server console.

