/**
 * =============================================================================
 * Viper
 * Copyright (C) 2007-2010 Zach "theY4Kman" Kanzler
 * Copyright (C) 2004-2007 AlliedModders LLC.  All rights reserved.
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

#ifndef _INCLUDE_VIPER_PY_DATATYPES_H_
#define _INCLUDE_VIPER_PY_DATATYPES_H_

#include <Python.h>

extern PyTypeObject datatypes__VectorType;
extern PyTypeObject datatypes__ColorType;

struct datatypes__Vector
{
    PyObject_HEAD
    
    float x, y, z;
};

struct datatypes__Color
{
    PyObject_HEAD
    
    int r, g, b, a;
};

#endif//_INCLUDE_VIPER_PY_DATATYPES_H_

