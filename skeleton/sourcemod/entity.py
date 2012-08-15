
class Entity:
  '''Contains methods and members to manipulate an edict and the entity it describes.'''
  
  '''This edict's classname.'''
  classname = None
  '''Datamaps for this Entity.'''
  datamaps = None
  '''The flags of the edict. These are not the same as entity flags.'''
  edict_flags = None
  '''This entity's networkable serverclass name. This is not the same as the classname and is used for networkable state changes.'''
  netclass = None
  '''SendProps for this Entity.'''
  sendprops = None
  def is_networkable(self):
    '''is_networkable() -> bool
    
    Returns whether or not the entity is networkable.
    @rtype: bool
    @return: True if networkable, false otherwise.'''
    pass
  
  def is_valid(self):
    '''is_valid() -> bool
    
    Returns whether or not the entity is valid.
    @rtype: bool
    @return: True if valid, false otherwise.'''
    pass
  
  def remove(self):
    '''remove()
    
    Removes the edict from the world.'''
    pass
  

class EntityProps:
  '''Retrieves sendprops and datamaps from an entity.'''
  

class EntityPropsArray:
  '''Retrieves data from sendprop and datamap arrays.'''
  

def create_edict():
  '''create_edict() -> Entity
  
  Creates a new edict (the basis of a networkable entity).
  
  @rtype: sourcemod.entity.Entity
  @return: A valid Entity object on success, None otherwise.'''
  pass

def get_entity_count():
  '''get_entity_count() -> int
  
  Returns the number of entities in the server
  @rtype: int
  @return: Number of entities in the server.'''
  pass

def get_max_entities():
  '''get_max_entities() -> int
  
  Returns the maximum number of entities.
  @rtype: int
  @return: Maximum number of entities.'''
  pass

