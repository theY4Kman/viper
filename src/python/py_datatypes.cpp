/**
 * =============================================================================
 * Viper
 * Copyright (C) 2008-2009 Zach "theY4Kman" Kanzler
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

#include <Python.h>
#include <structmember.h>
#include "py_datatypes.h"

static PyObject *
datatypes__Vector__repr__(datatypes__Vector *self)
{
    return PyString_FromFormat("Vector(%f, %f, %f)", self->x, self->y, self->z);
}

static PyObject *
datatypes__Vector__richcmp__(datatypes__Vector *self, PyObject *b, int op)
{
    PyObject *pyfloat = NULL;
    if (!PyObject_IsInstance(b, (PyObject *)self->ob_type) &&
        (pyfloat = PyNumber_Float(b)) == NULL)
    {
        return PyErr_Format(PyExc_TypeError, "expected float or Vector, found %s",
            b->ob_type->tp_name);
    }
    
    /* Vector comparison */
    if (pyfloat == NULL)
    {
        datatypes__Vector *vec = (datatypes__Vector *)b;
        
#define ReturnVectorRichCmp(op) \
            return PyBool_FromLong((self->x op vec->x) && \
                                   (self->y op vec->y) && \
                                   (self->z op vec->z))
        
        switch (op)
        {
        case Py_LT: ReturnVectorRichCmp(<);
        case Py_LE: ReturnVectorRichCmp(<=);
        case Py_EQ: ReturnVectorRichCmp(==);
        
        case Py_NE:
            return PyBool_FromLong((self->x != vec->x) ||
                                   (self->y != vec->y) ||
                                   (self->z != vec->z));
        
        case Py_GT: ReturnVectorRichCmp(>);
        case Py_GE: ReturnVectorRichCmp(>=);
        default:
            return Py_NotImplemented;
        };

#undef ReturnVectorRichCmp
    }
    /* Scalar <-> Vector comparison */
    else
    {
        float scal = PyFloat_AsDouble(pyfloat);
        Py_DECREF(pyfloat);
        
#define ReturnScalarRichCmp(op) \
            return PyBool_FromLong((self->x op scal) && \
                                   (self->y op scal) && \
                                   (self->z op scal))
        switch (op)
        {
        case Py_LT: ReturnScalarRichCmp(<);
        case Py_LE: ReturnScalarRichCmp(<=);
        case Py_EQ: ReturnScalarRichCmp(==);
        
        case Py_NE:
            return PyBool_FromLong((self->x != scal) ||
                                   (self->y != scal) ||
                                   (self->z != scal));
        
        case Py_GT: ReturnScalarRichCmp(>);
        case Py_GE: ReturnScalarRichCmp(>=);
        default:
            return Py_NotImplemented;
        };
#undef ReturnScalarRichCmp
    }
}

static int
datatypes__Vector__init__(datatypes__Vector *self, PyObject *args,
                          PyObject *keywds)
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    
    static char *keywdlist[] = {"x", "y", "z", NULL};
    
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "|fff", keywdlist, &x, &y, &z))
        return -1;
    
    self->x = x;
    self->y = y;
    self->z = z;
    
    return 0;
}

static PyMemberDef datatypes__Vector__members[] = {
    {"x", T_FLOAT, offsetof(datatypes__Vector, x)},
    {"y", T_FLOAT, offsetof(datatypes__Vector, y)},
    {"z", T_FLOAT, offsetof(datatypes__Vector, z)},
    {NULL}
};

PyTypeObject datatypes__VectorType = {
    PyObject_HEAD_INIT(&PyType_Type)
    0,                                                      /*ob_size*/
    "sourcemod.datatypes.Vector",                           /*tp_name*/
    sizeof(datatypes__Vector),                              /*tp_basicsize*/
    0,                                                      /*tp_itemsize*/
    0,                                                      /*tp_dealloc*/
    0,                                                      /*tp_print*/
    0,                                                      /*tp_getattr*/
    0,                                                      /*tp_setattr*/
    0,                                                      /*tp_compare*/
    (reprfunc)datatypes__Vector__repr__,                    /*tp_repr*/
    0,                                                      /*tp_as_number*/
    0,                                                      /*tp_as_sequence*/
    0,                                                      /*tp_as_mapping*/
    0,                                                      /*tp_hash */
    0,                                                      /*tp_call*/
    0,                                                      /*tp_str*/
    0,                                                      /*tp_getattro*/
    0,                                                      /*tp_setattro*/
    0,                                                      /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE|Py_TPFLAGS_HAVE_RICHCOMPARE,/*tp_flags*/
    "A 3D vector!",                                         /* tp_doc */
    0,                                                      /* tp_traverse */
    0,                                                      /* tp_clear */
    (richcmpfunc)datatypes__Vector__richcmp__,              /* tp_richcompare */
    0,                                                      /* tp_weaklistoffset */
    0,                                                      /* tp_iter */
    0,                                                      /* tp_iternext */
    0,                                                      /* tp_methods */
    datatypes__Vector__members,                             /* tp_members */
    0,                                                      /* tp_getset */
    0,                                                      /* tp_base */
    0,                                                      /* tp_dict */
    0,                                                      /* tp_descr_get */
    0,                                                      /* tp_descr_set */
    0,                                                      /* tp_dictoffset */
    (initproc)datatypes__Vector__init__,                    /* tp_init */
    0,                                                      /* tp_alloc */
    &PyType_GenericNew,                                     /* tp_new */
};

PyObject *
initdatatypes(void)
{
    if (PyType_Ready(&datatypes__VectorType) < 0)
        return NULL;
    
    PyObject *datatypes = Py_InitModule3("datatypes", NULL, "Generic data types.");
    
    Py_INCREF((PyObject *)&datatypes__VectorType);
    PyModule_AddObject(datatypes, "Vector", (PyObject *)&datatypes__VectorType);
    
    return datatypes;
}

PyObject *
CreatePyVector(float x, float y, float z)
{
    datatypes__Vector *vec = (datatypes__Vector *)datatypes__VectorType.tp_new(
        &datatypes__VectorType, NULL, NULL);
    vec->x = x;
    vec->y = y;
    vec->z = z;
    
    return (PyObject *)vec;
}

