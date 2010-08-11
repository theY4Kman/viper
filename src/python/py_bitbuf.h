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
#include "viper_globals.h"

extern PyTypeObject bitbuf__BitBufType;

struct bitbuf__BitBuf
{
    PyObject_HEAD
    
    // The buffer shared between read and write
    void *buffer;
    // The max length, in bytes, of the buffer
    int nBytes;
    
    bf_write *write;
    bf_read *read;
};

#endif//_INCLUDE_PYTHON_BITBUF_H_

