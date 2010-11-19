/**
 * =============================================================================
 * Viper
 * Copyright (C) 2010 Brandon "monokrome" Stoner
 * Copyright (C) 2008-2010 Zach "theY4Kman" Kanzler
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

#if defined(WIN32)
/* When delay loading in Windows, data variables are unavailable, so we must
 * define them ourselves.
 */
#   undef Py_None
#   undef Py_True
#   undef Py_False
#   undef Py_NotImplemented
    extern PyObject *Py_None;
    extern PyObject *Py_True;
    extern PyObject *Py_False;
    extern PyObject *Py_NotImplemented;
    
    extern PyTypeObject *_PyInt_Type;
    extern PyTypeObject *_PyString_Type;
    extern PyTypeObject *_PyFloat_Type;
    extern PyTypeObject *_PyType_Type;
    extern PyTypeObject *_PyLong_Type;
    extern PyTypeObject *_PyTuple_Type;
    extern PyTypeObject *_PyBool_Type;
    extern PyTypeObject *_PyDict_Type;
    
#   undef PyInt_Check
#   undef PyFloat_Check
#   undef PyString_Check
#   undef PyType_Check
#   undef PyLong_Check
#   undef PyTuple_Check
#   undef PyBool_Check
#   undef PyDict_Check

#   define PyInt_Check(op) PyObject_TypeCheck(op, _PyInt_Type)
#   define PyFloat_Check(op) PyObject_TypeCheck(op, _PyFloat_Type)
#   define PyString_Check(op) PyObject_TypeCheck(op, _PyString_Type)
#   define PyType_Check(op) PyObject_TypeCheck(op, _PyType_Type)
#   define PyLong_Check(op) PyObject_TypeCheck(op, _PyLong_Type)
#   define PyTuple_Check(op) PyObject_TypeCheck(op, _PyTuple_Type)
#   define PyDict_Check(op) PyObject_TypeCheck(op, _PyDict_Type)
#   define PyBool_Check(x) ((x)->ob_type == _PyBool_Type)

    extern PyObject *_PyExc_TypeError;
    extern PyObject *_PyExc_IndexError;
    extern PyObject *_PyExc_IOError;
    extern PyObject *_PyExc_RuntimeError;
    extern PyObject *_PyExc_KeyError;
    extern PyObject *_PyExc_RuntimeWarning;
    extern PyObject *_PyExc_SystemExit;
    extern PyObject *_PyExc_StopIteration;
#else
#   define _PyInt_Type &PyInt_Type
#   define _PyString_Type &PyString_Type
#   define _PyFloat_Type &PyFloat_Type
#   define _PyType_Type &PyType_Type
#   define _PyLong_Type &PyLong_Type
#   define _PyTuple_Type &PyTuple_Type
#   define _PyBool_Type &PyBool_Type
#   define _PyDict_Type &PyDict_Type

#   define _PyExc_TypeError PyExc_TypeError
#   define _PyExc_IndexError PyExc_IndexError
#   define _PyExc_IOError PyExc_IOError
#   define _PyExc_RuntimeError PyExc_RuntimeError
#   define _PyExc_KeyError PyExc_KeyError
#   define _PyExc_RuntimeWarning PyExc_RuntimeWarning
#   define _PyExc_SystemExit PyExc_SystemExit
#   define _PyExc_StopIteration PyExc_StopIteration
#endif

