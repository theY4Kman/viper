================================================
:mod:`sourcemod.console` -- Console interaction.
================================================

..  module:: sourcemod.console
..  moduleauthor:: Zach "theY4Kman" <they4kman@gmail.com>

Constants
=========

..  data:: Plugin_Continue
           Plugin_Stop
           Plugin_Handled
    
    These are frequently used as return values in callback functions to instruct Viper how to handle a situation. Generally, `Plugin_Continue` means to continue processing other callbacks, `Plugin_Stop` means to stop whatever processing is going on, and `Plugin_Handled` means to stop processing because we've done our job.

FCVAR Constants
---------------

These constants describe how the Source engine should handle console commands and variables. Most of the descriptions below come directly from the Source SDK.

..  list-table::
    :widths: 30 145

    * - **Name**
      - **Description**
    * - FCVAR_NONE
      - The default; no flags at all
    * - FCVAR_UNREGISTERED
      - If this is set, don't add to linked list, etc.
    * - FCVAR_LAUNCHER
      - Defined by launcher.
    * - FCVAR_GAMEDLL
      - Defined by the game DLL.
    * - FCVAR_CLIENTDLL
      - Defined by the client DLL.
    * - FCVAR_MATERIAL_SYSTEM
      - Defined by the material system.
    * - FCVAR_PROTECTED
      - A server cvar that contains sensitive data, such as a password. When its value is requested, it sends 1 if it's not empty/zero, and 0 otherwise.
    * - FCVAR_SPONLY
      - This cvar cannot be changed by clients connected to a multiplayer server.
    * - FCVAR_ARCHIVE
      - Set to cause this cvar to be saved to vars.rc
    * - FCVAR_NOTIFY
      - Notifies players when the cvar's value is changed. For example, this flag is set for *sv_cheats*
    * - FCVAR_USERINFO
      - Changes the client's info string.
    * - FCVAR_PRINTABLEONLY
      - This cvar's string cannot contain unprintable characters (e.g., used for player name, etc.)
    * - FCVAR_UNLOGGED
      - If the cvar has FCVAR_SERVER set, don't log changes to the log file / console if we are creating a log
    * - FCVAR_NEVER_AS_STRING
      - Never try to print that cvar.
    * - FCVAR_REPLICATED
      - Server setting enforced on clients.
    * - FCVAR_CHEAT
      - Only useable in singleplayer / debug / multiplayer & sv_cheats 1
    * - FCVAR_STUDIORENDER
      - Defined by the studiorender system.
    * - FCVAR_DEMO
      - Record this cvar when starting a demo file.
    * - FCVAR_DONTRECORD
      - Don't record this command in demo files.
    * - FCVAR_PLUGIN
      - Defined by a 3rd party plugin.
    * - FCVAR_DATACACHE
      - Defined by the datacache system.
    * - FCVAR_TOOLSYSTEM
      - Defined by an IToolSystem library.
    * - FCVAR_FILESYSTEM
      - Defined by the file system.
    * - FCVAR_NOT_CONNECTED
      - Cvar cannot be changed by a client that is connected to a server.
    * - FCVAR_SOUNDSYSTEM
      - Defined by the soundsystem library.
    * - FCVAR_ARCHIVE_XBOX
      - Cvar written to config.cfg on the Xbox.
    * - FCVAR_INPUTSYSTEM
      - Defined by the inputsystem DLL.
    * - FCVAR_NETWORKSYSTEM
      - Defined by the network system.
    * - FCVAR_VPHYSICS
      - Defined by vphysics.

Functions
=========

..  function:: print_to_server(message)
    
    Sends a message to the server console. This is the same as ``print message``, so long as `sys.stdout` remains unchanged by your plug-in.

..  function:: reg_concmd(name, callback[, description="", flags=0])
    
    Registers a new console command or hooks an existing one.
    
    :type   name: str
    :param name: Name of the ConCommand
    :type  callback: callable
    :param callback: A function to call when the ConCommand is executed.
    :type  description: str
    :param description: (Optional) Description of the ConCommand
    :type  flags: FCVAR Constants
    :param flags: Flags that change how the ConCommand is handled.
    
    *name* is the name of the console command. *callback* is the function to call when the console command is executed. This function should have one parameter: a ConCommandReply object.
