===============
Global Forwards
===============

Global forwards are :class:`Forwards <sourcemod.forwards.Forward>` that are managed by Viper and can be hooked anywhere in any plug-in. Here is a list of global forwards provided by Viper:

Global fowards can be hooked with the :func:`forwards.register <sourcemod.forwards.register>` function. Here's an example with the ``client_connect``::

    from sourcemod import forwards
    def on_client_connect(client):
        if client.name == "theY4Kman":
            return False # Reject this client
        elif client.name == "sawce":
            return (False, "go away, sawce") # Reject this client with a message
        else:
            return True # Allow everyone else
    
    forwards.register("client_connect", on_client_connect)

+------------------------+-------------------------------+---------------------------------------------------+
| Global Forward Name    | Arguments                     | Description                                       |
+========================+===============================+===================================================+
| client_connect         | (client)                      | Return True to allow connection and False         |
|                        |                               | to reject. If you want to include a               |
|                        |                               | rejection message, return a tuple containing      |
|                        |                               | the boolean and a rejection message, e.g.:        |
|                        |                               | ``(False, "REJECTED")``                           |
+------------------------+-------------------------------+---------------------------------------------------+
| client_connected       | (client)                      |                                                   |
+------------------------+-------------------------------+---------------------------------------------------+
| client_put_in_server   | (client)                      |                                                   |
+------------------------+-------------------------------+---------------------------------------------------+
| client_disconnecting   | (client)                      |                                                   |
+------------------------+-------------------------------+---------------------------------------------------+
| client_disconnected    | (client)                      |                                                   |
+------------------------+-------------------------------+---------------------------------------------------+
| client_authorized      | (client, auth)                |                                                   |
+------------------------+-------------------------------+---------------------------------------------------+
| client_pre_admin_check | (client)                      | Return True to continue with admin checks,        |
|                        |                               | or False to stop and ignore them.                 |
+------------------------+-------------------------------+---------------------------------------------------+
| client_post_admin_check| (client)                      |                                                   |
+------------------------+-------------------------------+---------------------------------------------------+
| server_activate        | (clientMax)                   | ``clientMax`` is the max number of clients        |
|                        |                               | the server supports.                              |
+------------------------+-------------------------------+---------------------------------------------------+
| ban_client             | (client, ban_time, ban_flags, | At the moment, this forward is only called        |
|                        | ban_reason, ban_cmd,          | if  is called from Viper, not                     |
|                        | ban_source)                   | :meth:`Client.ban <sourcemod.clients.Client.ban>` |
|                        |                               | from SourcePawn. Return True if the ban is        |
|                        |                               | handled, and False to continue processing the ban.|
+------------------------+-------------------------------+---------------------------------------------------+