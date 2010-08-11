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

#ifndef _INCLUDE_VIPER_PY_CONSOLE_H_
#define _INCLUDE_VIPER_PY_CONSOLE_H_

#include <Python.h>
#include "viper_globals.h"
#include <IViperForwardSys.h>

extern PyTypeObject console__ConCommandReplyType;

typedef struct {
    PyObject_HEAD
    /* ConCommand arguments passed */
    PyListObject *args;
    /* Arg string */
    char const *argstring;
    /* Name of the ConCommand */
    char const *name;
    /* The client whom called the ConCommand */
    PyObject *client;
} console__ConCommandReply;

extern PyTypeObject console__ConVarType;

typedef struct {
    PyObject_HEAD
    
    char const *name;
    
    ConVar *pVar;
    IViperForward *cvarChangeHooks; /**< Forward associated with cvar */
    bool byViper;                   /**< Whether or not the cvar was created by a Viper plugin */
} console__ConVar;

#endif // _INCLUDE_VIPER_PY_CONSOLE_H_
