=================================================
:mod:`sourcemod.console` --- Console interaction.
=================================================

..  module:: sourcemod.console
..  moduleauthor:: Zach "theY4Kman" Kanzler <they4kman@gmail.com>

Module Contents
===============

..  _sm-reply-constants:

+--------------------------------------+------------------------------------------------+
| Flag                                 | Description                                    |
+======================================+================================================+
| .. data:: SM_REPLY_CONSOLE           | The command was executed in the console.       |
+--------------------------------------+------------------------------------------------+
| .. data:: SM_REPLY_CHAT              | The command was executed in chat (by "!cmd" or |
|                                      | "/cmd" -- depending on your server's chat      |
|                                      | trigger configuration)                         |
+--------------------------------------+------------------------------------------------+

..  function:: create_convar(name, value[, description[, flags[, min[, max]]]])
    
    Creates a new console variable.
    
    :type   name: str
    :param  name: Name of the ConVar
    :type   value: str
    :param  value: The value of the ConVar. As the internal representation of a ConVar's value is a string, stringization is left to the user.
    :type   description: str
    :param  description: ``(Optional)`` Description of the ConVar
    :type   flags: :ref:`fcvar-constants`
    :param  flags: ``(Optional)`` Flags that change how a ConVar is handled. Use :ref:`fcvar-constants`, such as FCVAR_CHEAT, etc.
    :type   min: float
    :param  min: The lowest numeric value this ConVar can be set to. Pass None to ignore this field.
    :type   max: float
    :param  max: The highest numeric value this ConVar can be set to. Pass None to ignore this field.
    :rtype: :ref:`ConVar <convar-objects>`
    :return: A valid ConVar object on success, None otherwise. See :ref:`convar-objects`


..  function:: find_convar(name)
    
    Finds the specified ConVar. Returns None if the ConVar could not be found.
    
    :type   name: str
    :param  name: The name of the ConVar to retrieve
    :rtype: :class:`sourcemod.console.ConVar`
    :return: A valid ConVar object on success, None otherwise.


..  function:: print_to_server(message)
    
    Sends a message to the server console. This is the same as ``print message``, so long as `sys.stdout` remains unchanged by your plug-in.


..  function:: reg_concmd(name, callback[, description="", flags=0])
    
    Registers a new console command or hooks an existing one.
    
    :type   name: str
    :param name: Name of the ConCommand
    :type  callback: callable
    :param callback: A function to call when the ConCommand is executed.
    :type  description: str
    :param description: ``(Optional)`` Description of the ConCommand
    :type  flags: :ref:`fcvar-constants`
    :param flags: Flags that change how the ConCommand is handled.
    
    *callback* is the function to call when the console command is executed. This callback should have one parameter: a :ref:`ConCommandReply <concommandreply-objects>` object.


..  function:: reg_srvcmd(name, callback[, description="", flags=0])
    
    This command is the same as :ref:`reg_concmd <sourcemod.console.reg_concmd>`, except that the command can only be called by the server console or RCON.    


..  function:: server_command(command)
    
    Executes a command is if it were run on the server console.
    
    :type   command: str
    :param  command: Command to execute


..  function:: server_execute()
    
    Executes every command in the server's command buffer now, rather than once per frame.


..  _fcvar-constants:

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


..  _convar-objects:

ConVar Objects
==============
    
..  attribute:: ConVar.flags
    
    The bitstring of FCVAR_* flags on this console variable

..  attribute:: ConVar.lower_bound
    
    The lower bound of this ConVar.

..  attribute:: ConVar.name
    
    *Read-only!* The name of the ConVar.

..  attribute:: ConVar.upper_bound
    
    The upper bound of this ConVar.

..  attribute:: ConVar.value
    
    The value of the ConVar. This will always be a string. If you try to set it as something else, it will try to str() it.


..  method:: ConVar.hook_change(callback)
    
    Creates a hook that is called when this console variable's value is changed.
    
    :type   callback: callable
    :param  callback: The function to call when the ConVar is changed.
    
    The callback should have the prototype ``callback(cvar, oldvalue, newvalue)``, where *cvar* is the ConVar object representing the ConVar that was changed, *oldvalue* is the previous value of the ConVar as a string, and *newvalue* is the value being assigned to the ConVar as a string.

..  method:: ConVar.reset()
    
    Resets the console variable to its default value.

..  method:: ConVar.unhook_change(callback)
    
    Removes a ConVar change hook from the update list. *callback* will no longer be called when the ConVar is changed.
    
    :type   callback: callable
    :param  callback: The active hook to remove
    :raises ViperException: No active hook on the ConVar, or an invalid or unregistered callback supplied.


..  _concommandreply-objects:

ConCommandReply Objects
=======================

..  attribute:: ConCommandReply.name
    
    The name of the ConCommand.

..  attribute:: ConCommandReply.args
    
    The arguments passed when the ConCommand was executed, as a list.

..  attribute:: ConCommandReply.argstring
    
    The full argument string sent to the server. This includes any quotes, as well.

..  attribute:: ConCommandReply.client
    
    The :ref:`Client <client-object>` whom executed the ConCommand.

..  attribute:: ConCommandReply.reply_to
    
    How the command was executed. This is either :ref:`SM_REPLY_CHAT <sm-reply-constants>` or :ref:`SM_REPLY_CONSOLE <sm-reply-constants>`

..  method:: ConCommandReply.reply(message)
    
    Replies to the client whom executed the ConCommand in the way they executed the ConCommand (either in the console or in chat).
    
    :type   message: str
    :param  message: The message to send