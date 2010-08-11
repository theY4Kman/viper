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

#include <Python.h>
#include "py_bitbuf.h"

/* Magic methods */
PyObject *
bitbuf__BitBuf__str__(bitbuf__BitBuf *self)
{
    return PyString_FromFormat("<BitBuf object at: %p>", self);
}

/* Get/Sets */
#if 0
PyObject *
bitbuf__BitBuf__num_bytes_leftget(bitbuf__BitBuf *self)
{
    if (self->buffer == NULL)
    {
        PyErr_SetString(g_pViperException, "Invalid bitbuf buffer.");
        return NULL;
    }
    
    if (self->read == NULL)
        self->read = new bf_read(self->buffer, self->nBytes);
    
    return PyInt_FromLong();
}
#endif

PyGetSetDef bitbuf__BitBuf__getsets[] = {
#if NOT_IMPLEMENTED_YET
    {"num_bytes_left", (getter)bitbuf__BitBuf__num_bytes_leftget, NULL,
        "The number of bytes left in a bitbuffer."},
#endif
    {NULL}
};

PyMethodDef bitbuf__BitBuf__methods[] = {
#ifdef NOT_IMPLEMENTED_YET
    {"write_bool",},
    {"write_byte",},
    {"write_char",},
    {"write_short",},
    {"write_word",},
    {"write_num",},
    {"write_bool",},
    {"write_bool",},
#endif
    {NULL, NULL, 0, NULL}
};

PyTypeObject bitbuf__BitBufType = {
    PyObject_HEAD_INIT(_PyType_Type)
    0,                                              /*ob_size*/
    "sourcemod.bitbuf.BitBuf",                      /*tp_name*/
    sizeof(bitbuf__BitBuf),                         /*tp_basicsize*/
    0,                                              /*tp_itemsize*/
0,//    (destructor)bitbuf__BitBuf__del__,              /*tp_dealloc*/
    0,                                              /*tp_print*/
    0,                                              /*tp_getattr*/
    0,                                              /*tp_setattr*/
    0,                                              /*tp_compare*/
    0,                                              /*tp_repr*/
    0,                                              /*tp_as_number*/
    0,                                              /*tp_as_sequence*/
    0,                                              /*tp_as_mapping*/
    0,                                              /*tp_hash */
    0,                                              /*tp_call*/
    (reprfunc)bitbuf__BitBuf__str__,                /*tp_str*/
    0,                                              /*tp_getattro*/
    0,                                              /*tp_setattro*/
    0,                                              /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE,         /*tp_flags*/
    /* tp_doc */
    "Interact with Source BitBuffers.",
    0,                                              /* tp_traverse */
    0,                                              /* tp_clear */
    0,                                              /* tp_richcompare */
    0,                                              /* tp_weaklistoffset */
    0,                                              /* tp_iter */
    0,                                              /* tp_iternext */
    bitbuf__BitBuf__methods,                        /* tp_methods */
    0,                                              /* tp_members */
    bitbuf__BitBuf__getsets,                        /* tp_getset */
    0,                                              /* tp_base */
    0,                                              /* tp_dict */
    0,                                              /* tp_descr_get */
    0,                                              /* tp_descr_set */
    0,                                              /* tp_dictoffset */
};

PyObject *
initbitbuf(void)
{
    Py_INCREF(_PyType_Type);
    bitbuf__BitBufType.ob_type = _PyType_Type;
    if (PyType_Ready(&bitbuf__BitBufType) < 0)
        return NULL;
    
    PyObject *bitbuf = Py_InitModule3("bitbuf", NULL,
        "Manipulation of Source bit buffers");
    
    return bitbuf;
}

