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

/**
 * @file python/init.h
 * @brief Holds the declarations of the Python module initialization functions
 */

#ifndef _INCLUDE_VIPER_PYTHON_INIT_H_
#define _INCLUDE_VIPER_PYTHON_INIT_H_

/**
 * Initializes the standard Viper library module, `sourcemod`,
 * as well as initializes and adds submodules, such as `console` and `clients`
 */
PyObject *initsourcemod(void);

/** Initializes the module and returns it. */
PyObject *initconsole(void);
PyObject *initfiles(void);
PyObject *initforwards(void);
PyObject *initevents(void);
PyObject *initclients(void);
PyObject *initentity(void);
PyObject *inithalflife(void);
PyObject *initkeyvalues(void);
PyObject *initdatatypes(void);
PyObject *initusermessages(void);
PyObject *initbitbuf(void);
PyObject *initnatives(void);

#endif /* _INCLUDE_VIPER_PYTHON_INIT_H_ */

