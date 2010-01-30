================================================
:mod:`sourcemod.entity` --- Entity manipulation.
================================================

..  module:: sourcemod.entity
..  moduleauthor:: Zach "theY4Kman" Kanzler <they4kman@gmail.com>

Module Contents
===============

..  function:: create_edict()
    
    Creates a new edict (the basis of a networkable entity).

    :rtype: :ref:`Entity object <entity-object>`
    :returns: A valid :ref:`Entity object <entity-object>` on success, None otherwise

..  function:: get_entity_count()

    Returns the number of entities in the server.

..  function:: get_max_entities()
    
    Returns the maximum number of entities.


..  _entity-object:

Entity Objects
==============

..  data:: Entity.classname
    
    This edict's classname.

..  data:: Entity.datamaps

    An array-like object to access the datamaps of this entity::
        
        >>> my_entity = my_client.entity
        <Entity 2: 'player' at: 0xB00B5>
        >>> my_entity.datamaps["a_datamap_key"]
        "Valid datamap response!"

..  data:: Entity.edict_flags
    
    The flags of the edict. These are *not* the same as entity flags.

..  data:: Entity.netclass
    
    This entity's networkable serverclass name. This is not the same as the classname and is used for networkable state changes.

..  data:: Entity.sendprops
    
    The send props of the entity in a nice array-like object. Viper is able to determine the type of the sendprop, and thus needs only the name of the property. For example, we can go back and forth between a gun and his owner over and over::
        
        >>> weap = my_client.entity.sendprops["m_hActiveWeapon"]
        <Entity 632: 'weapon_ak47' at: 0xBADD06>
        >>> if my_client.entity == weap.sendprops["m_hOwner"]: print True
        True

    Snazzy, eh? It even supports arrays inside sendprops, so you can do ``my_client.entity.sendprops["m_hMyWeapons"][3]``

..  method:: Entity.is_networkable()
    
    Returns whether or not the entity is networkable.

..  method:: Entity.is_valid()
    
    Returns whether or not the entity is valid.

..  method:: remove()

    Removes the edict from the world
