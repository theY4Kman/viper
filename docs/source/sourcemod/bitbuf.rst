=============================================
:mod:`sourcemod.bitbuf` --- Source BitBuffers
=============================================

..  module:: sourcemod.bitbuf
..  moduleauthor:: Zach "theY4Kman" Kanzler <they4kman@gmail.com>

Bitbuffers are used most prominently in usermessages. They store data that will be sent to clients.

..  class: BitBuf

BitBuf Class
============

Represents a writeable bitbuffer (bf_write). This class is solely for writing to a bitbuffer; nothing can be read from it. Viper is lazy in that nothing is actually written to the bitbuffer until it has to be. The benefit of being lazy in that respect is many usermessages can be processed at the same time, providing thread stability.

..  method:: write_angle(angle[, numBits=8])
    
    Writes a bit angle. ``angle`` should be a :keyword:`float`, and ``numBits`` is the optional number of bits to use.

..  method:: write_angles(angles)
    
    Writes a 3D angle vector. ``angles`` should be a list-like object with three floats, such as a :class:`datatypes.Vector <sourcemod.datatypes.Vector>`.

..  method:: write_bool(bit)

    Writes a single bit. ``bit`` should be a :keyword:`bool`.

..  method:: write_byte(byte)

    Writes a byte. ``byte`` should be an :keyword:`int`. Only the first 8 bits will be used.

..  method:: write_coord(coord)

    Writes a coordinate. ``coord`` should be a :keyword:`float`.

..  method:: write_char(char)

    Writes a character. ``char`` should be a :keyword:`str[1]` -- a string of length 1.

..  method:: write_entity(entity)

    Writes a single bit. ``entity`` should be either an :keyword:`int` containing the index of the entity, or an :class:`entity.Entity <sourcemod.entity.Entity>` object.

..  method:: write_float(num)

    Writes a floating point number. ``num`` should be a :keyword:`float`.

..  method:: write_num(num)

    Writes a normal integer. ``num`` should be an :keyword:`int`.

..  method:: write_short(short)

    Writes a 16-bit integer. ``short`` should be an :keyword:`int`. Only the first 16 bits will be used.

..  method:: write_string(string)

    Writes a string. ``string`` should be a :keyword:`str`.

..  method:: write_vec_coord(vec)

    Writes a 3D vector of coordinates. ``vec`` should be a list-like object with three floats, such as a :class:`datatypes.Vector <sourcemod.datatypes.Vector>`.

..  method:: write_vec_normal(vec)

    Writes a 3D normal vector. ``vec`` should be a list-like object with three floats, such as a :class:`datatypes.Vector <sourcemod.datatypes.Vector>`.

..  method:: write_word(word)

    Writes a 16-bit unsigned integer. ``word`` should be an :keyword:`int`. Only the first 16 bits will be used.


..  class: BitBufRead

BitBufRead Class
================

Represents a readable bitbuffer (bf_read). Nothing can be written to this bitbuffer; it is solely for reading from it.

..  data: num_bytes_left
    
    The number of bytes left in the readable bitbuffer.