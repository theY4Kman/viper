/**
 * =============================================================================
 * Viper
 * Copyright (C) 2007-2010 Zach "theY4Kman" Kanzler
 * Copyright (C) 2004-2007 AlliedModders LLC.
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _INCLUDE_PYTHON_BITBUF_H_
#define _INCLUDE_PYTHON_BITBUF_H_

#include <Python.h>
#include <bitbuf.h>
#include <sh_stack.h>
#include "viper_globals.h"

extern PyTypeObject bitbuf__BitBufType;

enum BitBufType
{
    BitBufType_Bool = 0,
    BitBufType_Byte,
    BitBufType_Char,
    BitBufType_Short,
    BitBufType_Word,
    BitBufType_Num,
    BitBufType_Float,
    BitBufType_String,
    BitBufType_Entity,
    BitBufType_Angle,
    BitBufType_Coord,
    BitBufType_VecCoord,
    BitBufType_VecNormal
};

/** Stores information about one piece of data in a bitbuf */
struct BitBufCell
{
    BitBufType type;
    
    union BitBufCellData
    {
        bool asBool; // Bool
        int asInt; //Char, Byte, Short, Entity, Word
        long asLong; // Num
        float asFloat; // Float, Coord
        char const *asString; // String
        float asVector[3]; // VecCoord, VecNormal
        
        struct
        {
            float value;
            int numbits;
        } asAngle;
    } data;
};

struct bitbuf__BitBuf
{
    PyObject_HEAD
    
    // Stores data to write to a bf_write
    SourceHook::CStack<BitBufCell *> cells;
};

extern PyTypeObject bitbuf__BitBufType;

extern PyObject *GetBitBuf();
extern bool BitBufToBfWrite(bitbuf__BitBuf *obj, bf_write *bitbuf);

#endif//_INCLUDE_PYTHON_BITBUF_H_

