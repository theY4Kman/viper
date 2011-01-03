/**
 * =============================================================================
 * Viper
 * Copyright (C) 2007-2011 Zach "theY4Kman" Kanzler
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

#ifndef _INCLUDE_PYTHON_KEYVALUES_H_
#define _INCLUDE_PYTHON_KEYVALUES_H_

#include <Python.h>
#include <viper_keyvalues.h>

extern PyTypeObject keyvalues__KeyValuesType;

struct keyvalues__KeyValues
{
    PyObject_HEAD
    
    KeyValues *kv;
};

/* Function to retrieve cached KeyValues objects */
PyObject *GetPyObjectFromKeyValues(KeyValues *kv);
void AddKeyValuesToCache(KeyValues *kv, PyObject *kvpy);
void RemoveKeyValuesFromCache(KeyValues *kv);

#endif//_INCLUDE_PYTHON_KEYVALUES_H_

