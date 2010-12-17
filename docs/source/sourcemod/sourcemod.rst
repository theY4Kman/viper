===============================================
:mod:`sourcemod` --- The standard Viper library
===============================================

..  module:: sourcemod
..  moduleauthor:: Zach "theY4Kman" Kanzler <they4kman@gmail.com>

This is the standard library provided by Viper. It is all written in C, and should be considered the fastest way to interact with the Source Engine.

..  exception:: ViperError
    
    Used for all Source-related errors. For example, this would be used to indicate an invalid entity, but not an object of the wrong type given to a function.

..  _result-types:

..  data:: Plugin_Continue
           Plugin_Stop
           Plugin_Handled
    
    These are frequently used as return values in callback functions to instruct Viper how to handle a situation. Generally, `Plugin_Continue` means to continue processing other callbacks, `Plugin_Stop` means to stop whatever processing is going on, and `Plugin_Handled` means to stop processing because we've done our job.

..  function:: get_game_path()
    
    Returns the full path to the game directory.
    
..  function:: get_sourcemod_path([path=""])
    
    Returns the full path to SourceMod. The ``path`` argument should be a string, and is appended to the SourceMod path that is returned. This is for convenience and readability.

..  class:: server_out
    
    Writes to the server console. An instance of this class is used to overwrite `sys.stdout` in order to provide `print` redirection to the server console.

    ..  method:: sourcemod.server_out.write(msg)
        
        Prints *msg* to the server console.
    
    ..  attribute:: sourcemod.server_out.real_stdout
        
        The value of sys.stdout before it was overridden by this class.

