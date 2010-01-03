=================================================
:mod:`sourcemod.clients` --- Client manipulation.
=================================================

..  module:: sourcemod.clients
..  moduleauthor:: Zach "theY4Kman" Kanzler <they4kman@gmail.com>

Module Contents
===============

..  data:: BANFLAG_AUTO
    
    Auto-detects whether to ban by steamid or IP

..  data:: BANFLAG_IP
    
    Always ban by IP

..  data:: BANFLAG_AUTHID
    
    Always ban by authstring (Steam ID) if possible

..  data:: BANFLAG_NOKICK
    
    Does not kick the client

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


..  _client-object:

Client Objects
==============

..  method:: Client.fake_command(cmd)
    
    Executes a client command on the server without being networked.
    
    :type   cmd: str
    :param  cmd: The command string to execute.

..  method:: Client.is_timing_out()
    
    Returns if the client is timing out.

..  method:: Client.kick([msg="" [, delay=True]])
    
    Disconnects a player from the server.
    
    :type   msg: str
    :param  msg: A message to show the user as a disconnect reason. Note that a period is automatically appended to the message by the engine.
    :type   delay: bool
    :param  delay: If True, the client is kicked in the next game frame. If False, the client is kicked immediately. The delay exists to prevent accidental engine crashes.

..  method:: Client.notify_post_admin_check()
    
    Signals that a player has completed post-connection admin checks. Has no effect if the player has already had this event signalled. Note: This must be sent even if no admin id was assigned.

..  method:: Client.print_center(message)
    
    Prints a message to this client in the center of the screen.
    
    :type   message: str
    :param  message: The message to print.

..  method:: Client.print_chat(message)
    
    Prints a message to this client's chat area.
    
    :type   message: str
    :param  message: The message to print.

..  method:: Client.print_console(message)
    
    Prints a message to this client's console.
    
    :type   message: str
    :param  message: The message to print.

..  method:: Client.print_hint(message)
    
    Prints a message to this client inside a hint box.
    
    :type   message: str
    :param  message: The message to print.

..  method:: Client.set_fake_client_convar(convar, value)
    
    Sets a convar value on a fake client.
    
    :type   convar: str
    :param  convar: The ConVar's name
    :type   value: str
    :param  value: The value to set the ConVar to.

..  method:: Client.show_vgui_panel(name[, kv=None[, show=True]])
    
    Show a VGUI panel to the client.
    
    :type   name: str
    :param  name: Panel type name (see viewport_panel_names.h for a list of panel names)
    :type   kv: :ref:`KeyValues <keyvalues-object>`
    :param  kv: A :ref:`KeyValues <keyvalues-object>` with all the data for the panel setup. The data inside the :ref:`KeyValues <keyvalues-object>` depends on the panel type.
    :type   show: bool
    :param  show: True to show the panel, or False to remove it from the client's screen.

Attributes
----------

..  data:: Client.abs_angles
    
    The client's angles vector.

    :except sourcemod.ViperError: Invalid client, client not in-game, or no mod support. 

..  data:: Client.abs_origin
    
    The client's origin vector.

    :except sourcemod.ViperError: Invalid client, client not in-game, or no mod support.

..  data:: Client.alive
    
    Whether the client is alive or dead.

    :except sourcemod.ViperError: Invalid client, client not in-game, or no mod support.

..  data:: Client.avg_choke

    The client's average packet choke. Value is a percentage ranging from 0.0 to 1.0

    :except sourcemod.ViperError: Invalid client, client not in-game, or client is a fake client.
    
..  data:: Client.avg_choke_in
    
    The client's average incoming packet choke. Value is a percentage ranging from 0.0 to 1.0.

    :except sourcemod.ViperError: Invalid client, client not in-game, or client is a fake client.

..  data:: Client.avg_choke_out
    
    The client's average outgoing packet choke. Value is a percentage ranging from 0.0 to 1.0.

    :except sourcemod.ViperError: Invalid client, client not in-game, or client is a fake client.

..  data:: Client.avg_data
    
    The client's data flow, incoming and outgoing, in bytes per second.

    :except sourcemod.ViperError: Invalid client, client not in-game, or no mod support.

..  data:: Client.avg_data_in
    
    The client's incoming data flow in bytes per second.

    :except sourcemod.ViperError: Invalid client, client not in-game, or no mod support.
    
..  data:: Client.avg_data_out
    
    The client's outgoing data flow in bytes per second.

    :except sourcemod.ViperError: Invalid client, client not in-game, or no mod support.

..  data:: Client.avg_latency
    
    The client's average packet latency, both incoming and outgoing, in seconds.

    :except sourcemod.ViperError: Invalid client, client not in-game, or client is a fake client.

..  data:: Client.avg_latency_in
    
    The client's average incoming packet latency in seconds.

    :except sourcemod.ViperError: Invalid client, client not in-game, or client is a fake client.

..  data:: Client.avg_latency_out
    
    The client's average outgoing packet latency in seconds.

    :except sourcemod.ViperError: Invalid client, client not in-game, or client is a fake client.

..  data:: Client.avg_loss
    
    The client's average packet loss. Value is a percentage ranging from 0.0 to 1.0.

    :except sourcemod.ViperError: Invalid client, client not in-game, or client is a fake client.

..  data:: Client.avg_loss_in
    
    The client's average incoming packet loss. Value is a percentage ranging from 0.0 to 1.0.

    :except sourcemod.ViperError: Invalid client, client not in-game, or client is a fake client.

..  data:: Client.avg_loss_out
    
    The client's average outgoing packet loss. Value is a percentage ranging from 0.0 to 1.0.

    :except sourcemod.ViperError: Invalid client, client not in-game, or client is a fake client.

..  data:: Client.avg_packets
    
    The client's average packet frequency, incoming and outgoing, in packets per second.

    :except sourcemod.ViperError: Invalid client, client not in-game, or client is a fake client.

..  data:: Client.avg_packets_in
    
    The client's average incoming packet frequency, in packets per second.

    :except sourcemod.ViperError: Invalid client, client not in-game, or client is a fake client.

..  data:: Client.avg_packets_out
    
    The client's average outgoing packet frequency, in packets per second.

    :except sourcemod.ViperError: Invalid client, client not in-game, or client is a fake client.

..  data:: Client.data_rate

    The client's send date rate in bytes per second

    :except sourcemod.ViperError: Invalid client, client not in-game, or client is a fake client.

..  data:: Client.deaths

    The player's death count.

    :except sourcemod.ViperError: Invalid client, client not in-game, or no mod support.

..  data:: Client.entity

    The :ref:`Entity <entity-object>` of the Client.

..  data:: Client.fake

    Whether the client is fake or not.

    :except sourcemod.ViperError: Invalid client or client not connected.

..  data:: Client.frags

    The player's frag count.

    :except sourcemod.ViperError: Invalid client, client not in-game, or no mod support.

..  data:: Client.health

    The health of the player.

    :except sourcemod.ViperError: Invalid client, client not in-game, or no mod support.

..  data:: Client.ip

    The IP address of this client.

    :except sourcemod.ViperError: Invalid client or client not connected.

..  data:: Client.lang_id

    The client's language ID as an int.

    :except sourcemod.ViperError: Invalid client or client not connected.

..  data:: Client.latency

    The client's packet latency (RTT), both incoming and outgoing, in seconds. This is more accurate than the average latency, but jittery.

    :except sourcemod.ViperError: Invalid client, client not in-game, or client is a fake client.

..  data:: Client.latency_in

    The client's incoming packet latency (RTT) in seconds. This is more accurate than the average latency, but jittery.

    :except sourcemod.ViperError: Invalid client, client not in-game, or client is a fake client.

..  data:: Client.latency_out

    The client's outgoing packet latency (RTT) in seconds. This is more accurate than the average latency, but jittery.

    :except sourcemod.ViperError: Invalid client, client not in-game, or client is a fake client.

..  data:: Client.maxs

    The client's max size vector.

    :except sourcemod.ViperError: Invalid client, client not in-game, or no mod support.

..  data:: Client.mins

    The client's min size vector.

    :except sourcemod.ViperError: Invalid client, client not in-game, or no mod support.

..  data:: Client.model

    The client's model name.

    :except sourcemod.ViperError: Invalid client, client not in-game, or no mod support.

..  data:: Client.name

    The name of this client.

    :except sourcemod.ViperError: Invalid client or client not connected.

..  data:: Client.serial

    The client's unique serial identifier.

..  data:: Client.steamid

    The Steam ID of this client. This is None when the client is not authorized, yet.

    :except sourcemod.ViperError: Invalid client or client not connected.

..  data:: Client.team

    The player's team.

    :except sourcemod.ViperError: Invalid client, client not in-game, no mod support, or client not in-game.

..  data:: Client.time_connected

    The amount of time the client has been connected in seconds.

    :except sourcemod.ViperError: Invalid client, client not in-game, or client is a fake client.

..  data:: Client.userid

    The userid of this client.

    :except sourcemod.ViperError: Invalid client or client not connected.

..  data:: Client.weapon

    The player's weapon name.

    :except sourcemod.ViperError: Invalid client, client not in-game, or no mod support.


