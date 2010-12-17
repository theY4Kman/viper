=================================================
:mod:`sourcemod.clients` --- Client manipulation.
=================================================

..  module:: sourcemod.clients
..  moduleauthor:: Zach "theY4Kman" Kanzler <they4kman@gmail.com>

The :mod:`sourcemod.clients` module contains functions and objects having to do with clients. There may be different functions available in this module than SourceMod's `clients.inc`, because they fit in better here.

Constants
=========

..  _banflags:

These flags will be used in the clients.Client.ban() function, which is yet to be implemented.

+--------------------------------------+------------------------------------------------+
| Flag                                 | Description                                    |
+======================================+================================================+
| .. data:: BANFLAG_AUTO               | Auto-detects whether to ban by steamid or IP   |
+--------------------------------------+------------------------------------------------+
| .. data:: BANFLAG_IP                 | Always ban by IP                               |
+--------------------------------------+------------------------------------------------+
| .. data:: BANFLAG_AUTHID             | Always ban by authstring (Steam ID) if possible|
+--------------------------------------+------------------------------------------------+
| .. data:: BANFLAG_NOKICK             | Does not kick the client                       |
+--------------------------------------+------------------------------------------------+

..  _command-filter-flags:

Use COMMAND_FILTER flags with :ref:`process_target_string <sourcemod.clients.process_target_string>` to filter the players that will be chosen.

+--------------------------------------+-----------------------------------------------+
| Flag                                 | Description                                   |
+======================================+===============================================+
| .. data:: COMMAND_FILTER_ALIVE       | Only allow alive players                      |
+--------------------------------------+-----------------------------------------------+
| .. data:: COMMAND_FILTER_DEAD        | Only filter dead players                      |
+--------------------------------------+-----------------------------------------------+
| .. data:: COMMAND_FILTER_CONNECTED   | Allow players not fully in-game               |
+--------------------------------------+-----------------------------------------------+
| .. data:: COMMAND_FILTER_NO_IMMUNITY | Ignore immunity rules                         |
+--------------------------------------+-----------------------------------------------+
| .. data:: COMMAND_FILTER_NO_MULTI    | Do not allow multiple target patterns         |
+--------------------------------------+-----------------------------------------------+
| .. data:: COMMAND_FILTER_NO_BOTS     | Do not allow bots to be targetted             |
+--------------------------------------+-----------------------------------------------+

..  _command-targets:

The tuple :ref:`process_target_string <sourcemod.clients.process_target_string>` returns contains a `reason` field, which will be one of the constants below. These describe the reason why :ref:`process_target_string <sourcemod.clients.process_target_string>` failed or succeeded.
For implementation purposes, `reason` values larger than 0 succeeded, a value of 0 means no player was matched, and anything below 0 means players were found, but they didn't meet the COMMAND_FILTER flags passed.

+----------------------------------------+-----------------------------------------------+
| Flag                                   | Description                                   |
+========================================+===============================================+
| .. data:: COMMAND_TARGET_VALID         | Client passed the filter. The value of this   |
|                                        | flag is 1.                                    |
+----------------------------------------+-----------------------------------------------+
| .. data:: COMMAND_TARGET_NONE          | No target was found. The value of this flag   |
|                                        | is 0. The rest of the flags' values continue  |
|                                        | to decline by 1.                              |
+----------------------------------------+-----------------------------------------------+
| .. data:: COMMAND_TARGET_NOT_ALIVE     | Single client is not alive                    |
+----------------------------------------+-----------------------------------------------+
| .. data:: COMMAND_TARGET_NOT_DEAD      | Single client is not dead                     |
+----------------------------------------+-----------------------------------------------+
| .. data:: COMMAND_TARGET_NOT_IN_GAME   | Single client is not in game                  |
+----------------------------------------+-----------------------------------------------+
| .. data:: COMMAND_TARGET_IMMUNE        | Single client is immune                       |
+----------------------------------------+-----------------------------------------------+
| .. data:: COMMAND_TARGET_EMPTY_FILTER  | A multi-filter (such as @all) had no targets  |
+----------------------------------------+-----------------------------------------------+
| .. data:: COMMAND_TARGET_NOT_HUMAN     | Target was not human                          |
+----------------------------------------+-----------------------------------------------+
| .. data:: COMMAND_TARGET_AMBIGUOUS     | Partial name had too many targets             |
+----------------------------------------+-----------------------------------------------+

Functions
=========

..  function:: create_fake_client(name)
    
    Creates a fake client.
    
    :type   name: str
    :param  name: The name to assign to the fake client
    :rtype: :ref:`Client <client-object>`
    :returns: A valid :ref:`Client Object <client-object>` on success, None otherwise.

..  function:: get_client(index)
    
    Retrieves the :ref:`Client object <client-object>` with that client index.
    
    :type   index: int
    :param  index: The client index
    :rtype: :ref:`Client <client-object>`
    :returns: A valid :ref:`Client object <client-object>` on success, None if an invalid client index.
    
..  function:: get_client_count([in_game_only=True])
    
    Returns the number of clients put in the server.
    
    :type   in_game_only: bool
    :param  in_game_only: If False, players currently connecting are also counted.
    :rtype: int

..  function:: get_client_from_serial(serial)
    
    Retrieves a :ref:`Client <client-object>` by their serial number.
    
    :type   serial: int
    :param  serial: The :ref:`Client <client-object>`'s serial number
    :rtype: :ref:`Client <client-object>`
    :returns: A valid :ref:`Client object <client-object>` on success, None if an invalid client serial.
    
..  function:: get_client_from_userid(userid)
    
    Translates a userid index into a :ref:`Client <client-object>` object
    
    :type   userid: int
    :param  userid: Userid value
    :rtype: :ref:`Client <client-object>`
    :returns: A valid :ref:`Client object <client-object>` on success, None if an invalid userid.

..  function:: get_max_clients()
    
    Returns the maximum number of clients allowed on the server. This may return 0 if called before the global forward :ref:`on_map_start`.

..  function:: process_target_string(pattern, admin[, flags=0])
    
    Processes a generic target string and resolves it to a list of clients, or one client, based on filtering rules.
    
    This function returns a tuple containing all the useful data: ``(targets, target_name, target_name_is_multilingual, reason)``
    
    * ``targets`` is a list of all the clients matched. This will always be a list, even when only one client is matched.
    * ``target_name`` is a string containing a description of the targets matched. For instance, if one client is matched, ``target_name`` will be that client's name; if all the bots that are alive are matched, ``target_name`` will be "all alive bots".
    * ``target_name_is_multilingual`` is a boolean that is True if ``target_name`` is multilingual. That is, if it's a translated phrase. For single client matches, where ``target_name`` is the client matched, this will be False.
    * ``reason`` is an int containing a :ref:`COMMAND_TARGET <command-targets>` constant, describing the reason why no players were matched.
    
    Returning a tuple allows easier handling of the data::
    
      targets,target_name,tn_is_ml,reason = process_target_string("@all", 2, COMMAND_FILTER_ALIVE|COMMAND_FILTER_CONNECTED)
    
    :type   pattern: str
    :param  pattern: Target pattern to process.
    :type   admin: clients.Client or int
    :param  admin: The :ref:`clients.Client object <client-object>` or client index of the client to process the target string from.
    :type   flags: int
    :param  flags: :ref:`COMMAND_FILTER flags<command-filter-flags>` to filter the search.

..  _client-object:

Client Objects
==============

Client objects cannot be instantiated by themselves. Instead, use :func:`get_client(index) <sourcemod.clients.get_client>` to retrieve a client object.

..  method:: ban(time, flags, reason[, kickmsg="Kicked", cmd=None, source=0])
    
    Bans the client. ``time`` is the time, in minutes, to ban the client (0 = permanent).
    
    ``flags`` is a bitstring of :ref:`BANFLAG flags <banflags>` that will control the banning mechanism. If :data:`BANFLAG_AUTHID <sourcemod.clients.BANFLAG_AUTHID>` is set and no auth ID is available, the ban will fail, unless :ref:`BANFLAG_AUTO <sourcemod.clients.BANFLAG_AUTO>` is also set.
    
    ``reason`` is a string containing the reason for banning the client. ``kickmsg`` is the message to display to the player when they're kicked.
    
    ``cmd`` is a string containing the command used to initiate the ban in order to identify the source. If this is left empty, then the ban_client forward will not be called. ``source`` is a value that could be interpreted as the identity of the player whom was the source of the banning (not actually checked by Core).
    
    This returns ``True`` if the player was successfully banned, and ``False`` otherwise (the only case ``False`` is returned is mentioned above in the explanation of ``flags``).

..  method:: fake_command(cmd)
    
    Executes a client command on the server without being networked.
    
    :type   cmd: str
    :param  cmd: The command string to execute.

..  method:: is_connected()
    
    Returns whether the client is connected.

..  method:: is_timing_out()
    
    Returns whether the client is timing out.

..  method:: kick([msg="" [, delay=True]])
    
    Disconnects a player from the server.
    
    :type   msg: str
    :param  msg: A message to show the user as a disconnect reason. Note that a period is automatically appended to the message by the engine.
    :type   delay: bool
    :param  delay: If True, the client is kicked in the next game frame. If False, the client is kicked immediately. The delay exists to prevent accidental engine crashes.
    :rtype: bool
    :returns: True on successful kick, False if the player is already in the kick queue.

..  method:: notify_post_admin_check()
    
    Signals that a player has completed post-connection admin checks. Has no effect if the player has already had this event signalled. Note: This must be sent even if no admin id was assigned.

..  method:: print_center(message)
    
    Prints a message to this client in the center of the screen.
    
    :type   message: str
    :param  message: The message to print.

..  method:: print_chat(message)
    
    Prints a message to this client's chat area.
    
    :type   message: str
    :param  message: The message to print.

..  method:: print_console(message)
    
    Prints a message to this client's console.
    
    :type   message: str
    :param  message: The message to print.

..  method:: print_hint(message)
    
    Prints a message to this client inside a hint box.
    
    :type   message: str
    :param  message: The message to print.

..  method:: set_fake_client_convar(convar, value)
    
    Sets a convar value on a fake client.
    
    :type   convar: str
    :param  convar: The ConVar's name
    :type   value: str
    :param  value: The value to set the ConVar to.

..  method:: show_vgui_panel(name[, kv=None[, show=True]])
    
    Show a VGUI panel to the client.
    
    :type   name: str
    :param  name: Panel type name (see viewport_panel_names.h for a list of panel names)
    :type   kv: :class:`KeyValues <keyvalues-object>`
    :param  kv: A :class:`KeyValues <keyvalues-object>` with all the data for the panel setup. The data inside the :ref:`KeyValues <keyvalues-object>` depends on the panel type.
    :type   show: bool
    :param  show: True to show the panel, or False to remove it from the client's screen.

Attributes
----------

..  data:: index
    
    A number containing the index of the client. This is what SourcePawn uses for handling clients.

..  data:: abs_angles
    
    The client's angles vector.

    :except sourcemod.ViperError: Invalid client, client not in-game, or no mod support. 

..  data:: abs_origin
    
    The client's origin vector.

    :except sourcemod.ViperError: Invalid client, client not in-game, or no mod support.

..  data:: alive
    
    Whether the client is alive or dead.

    :except sourcemod.ViperError: Invalid client, client not in-game, or no mod support.

..  data:: avg_choke

    The client's average packet choke. Value is a percentage ranging from 0.0 to 1.0

    :except sourcemod.ViperError: Invalid client, client not in-game, or client is a fake client.
    
..  data:: avg_choke_in
    
    The client's average incoming packet choke. Value is a percentage ranging from 0.0 to 1.0.

    :except sourcemod.ViperError: Invalid client, client not in-game, or client is a fake client.

..  data:: avg_choke_out
    
    The client's average outgoing packet choke. Value is a percentage ranging from 0.0 to 1.0.

    :except sourcemod.ViperError: Invalid client, client not in-game, or client is a fake client.

..  data:: avg_data
    
    The client's data flow, incoming and outgoing, in bytes per second.

    :except sourcemod.ViperError: Invalid client, client not in-game, or no mod support.

..  data:: avg_data_in
    
    The client's incoming data flow in bytes per second.

    :except sourcemod.ViperError: Invalid client, client not in-game, or no mod support.
    
..  data:: avg_data_out
    
    The client's outgoing data flow in bytes per second.

    :except sourcemod.ViperError: Invalid client, client not in-game, or no mod support.

..  data:: avg_latency
    
    The client's average packet latency, both incoming and outgoing, in seconds.

    :except sourcemod.ViperError: Invalid client, client not in-game, or client is a fake client.

..  data:: avg_latency_in
    
    The client's average incoming packet latency in seconds.

    :except sourcemod.ViperError: Invalid client, client not in-game, or client is a fake client.

..  data:: avg_latency_out
    
    The client's average outgoing packet latency in seconds.

    :except sourcemod.ViperError: Invalid client, client not in-game, or client is a fake client.

..  data:: avg_loss
    
    The client's average packet loss. Value is a percentage ranging from 0.0 to 1.0.

    :except sourcemod.ViperError: Invalid client, client not in-game, or client is a fake client.

..  data:: avg_loss_in
    
    The client's average incoming packet loss. Value is a percentage ranging from 0.0 to 1.0.

    :except sourcemod.ViperError: Invalid client, client not in-game, or client is a fake client.

..  data:: avg_loss_out
    
    The client's average outgoing packet loss. Value is a percentage ranging from 0.0 to 1.0.

    :except sourcemod.ViperError: Invalid client, client not in-game, or client is a fake client.

..  data:: avg_packets
    
    The client's average packet frequency, incoming and outgoing, in packets per second.

    :except sourcemod.ViperError: Invalid client, client not in-game, or client is a fake client.

..  data:: avg_packets_in
    
    The client's average incoming packet frequency, in packets per second.

    :except sourcemod.ViperError: Invalid client, client not in-game, or client is a fake client.

..  data:: avg_packets_out
    
    The client's average outgoing packet frequency, in packets per second.

    :except sourcemod.ViperError: Invalid client, client not in-game, or client is a fake client.

..  data:: data_rate

    The client's send date rate in bytes per second

    :except sourcemod.ViperError: Invalid client, client not in-game, or client is a fake client.

..  data:: deaths

    The player's death count.

    :except sourcemod.ViperError: Invalid client, client not in-game, or no mod support.

..  data:: entity

    The :class:`Entity <sourcemod.entity.Entity>` of the Client.

..  data:: fake

    Whether the client is fake or not.

    :except sourcemod.ViperError: Invalid client or client not connected.

..  data:: frags

    The player's frag count.

    :except sourcemod.ViperError: Invalid client, client not in-game, or no mod support.

..  data:: health

    The health of the player.

    :except sourcemod.ViperError: Invalid client, client not in-game, or no mod support.

..  data:: ip

    The IP address of this client.

    :except sourcemod.ViperError: Invalid client or client not connected.

..  data:: lang_id

    The client's language ID as an int.

    :except sourcemod.ViperError: Invalid client or client not connected.

..  data:: latency

    The client's packet latency (RTT), both incoming and outgoing, in seconds. This is more accurate than the average latency, but jittery.

    :except sourcemod.ViperError: Invalid client, client not in-game, or client is a fake client.

..  data:: latency_in

    The client's incoming packet latency (RTT) in seconds. This is more accurate than the average latency, but jittery.

    :except sourcemod.ViperError: Invalid client, client not in-game, or client is a fake client.

..  data:: latency_out

    The client's outgoing packet latency (RTT) in seconds. This is more accurate than the average latency, but jittery.

    :except sourcemod.ViperError: Invalid client, client not in-game, or client is a fake client.

..  data:: maxs

    The client's max size vector.

    :except sourcemod.ViperError: Invalid client, client not in-game, or no mod support.

..  data:: mins

    The client's min size vector.

    :except sourcemod.ViperError: Invalid client, client not in-game, or no mod support.

..  data:: model

    The client's model name.

    :except sourcemod.ViperError: Invalid client, client not in-game, or no mod support.

..  data:: name

    The name of this client.

    :except sourcemod.ViperError: Invalid client or client not connected.

..  data:: serial

    The client's unique serial identifier.

..  data:: steamid

    The Steam ID of this client. This is None when the client is not authorized, yet.

    :except sourcemod.ViperError: Invalid client or client not connected.

..  data:: team

    The player's team.

    :except sourcemod.ViperError: Invalid client, client not in-game, no mod support, or client not in-game.

..  data:: time_connected

    The amount of time the client has been connected in seconds.

    :except sourcemod.ViperError: Invalid client, client not in-game, or client is a fake client.

..  data:: userid

    The userid of this client.

    :except sourcemod.ViperError: Invalid client or client not connected.

..  data:: weapon

    The player's weapon name.

    :except sourcemod.ViperError: Invalid client, client not in-game, or no mod support.


