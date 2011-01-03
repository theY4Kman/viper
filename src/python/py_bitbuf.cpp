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

#include <Python.h>
#include "py_bitbuf.h"
#include "py_entity.h"
#include "py_datatypes.h"

/* Tries to extract a vector out of a PyObject, as a list-like item, and as
 * a datatypes.Vector object.
 */
bool
PyObjectToVector(PyObject *obj, float vec[3])
{
    /* List-like item */
    if (PySequence_Check(obj))
    {
        if (PySequence_Length(obj) != 3)
        {
            PyErr_Format(_PyExc_TypeError, "Expected a list-like item of"
                " length 3, got length %d.", PySequence_Length(obj));
            return false;
        }
        
        for (unsigned int i=0; i<3; i++)
        {
            PyObject *fl = PySequence_GetItem(obj, i);
            if (PyInt_Check(fl))
                vec[i] = static_cast<float>(PyInt_AS_LONG(fl));
            else if (PyFloat_Check(fl))
                vec[i] = PyFloat_AS_DOUBLE(fl);
            else
            {
                PyErr_Format(_PyExc_TypeError, "Object in list-like object at"
                    " index %d is the wrong type (expected float, got \"%s\")",
                    i, fl->ob_type->tp_name);
                return false;
            }
        }
    }
    
    /* datatypes.Vector */
    else if (PyObject_IsInstance(obj, (PyObject *)&datatypes__VectorType))
    {
        datatypes__Vector *pyvec = (datatypes__Vector *)obj;
        vec[0] = pyvec->x;
        vec[1] = pyvec->y;
        vec[2] = pyvec->z;
    }
    
    /* sawce */
    else
    {
        PyErr_Format(_PyExc_TypeError, "object is not a vector (expected"
            " datatypes.Vector or list-like item of 3 floats, got \"%s\")",
            obj->ob_type->tp_name);
        return false;
    }
    
    return true;
}

static PyObject *
bitbuf__BitBuf__write_angle(bitbuf__BitBuf *self, PyObject *args,
                            PyObject *kwds)
{
    float num;
    int numBits = 8;
    
    static char *keywdlist[] = {"num", "numBits"};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "f|i", keywdlist, &num,
            &numBits))
        return NULL;
    
    BitBufCell *cell = new BitBufCell();
    cell->type = BitBufType_Angle;
    cell->data.asAngle.value = num;
    cell->data.asAngle.numbits = numBits;
    
    self->cells.push(cell);
    
    Py_RETURN_NONE;
}

static PyObject *
bitbuf__BitBuf__write_angles(bitbuf__BitBuf *self, PyObject *args)
{
    PyObject *py_vec;
    float vec[3] = {0.0, 0.0, 0.0};
    
    if (!PyArg_ParseTuple(args, "O", &py_vec))
        return NULL;
    
    if (!PyObjectToVector(py_vec, vec))
        return NULL;
    
    BitBufCell *cell = new BitBufCell();
    cell->type = BitBufType_Angles;
    cell->data.asVector[0] = vec[0];
    cell->data.asVector[1] = vec[1];
    cell->data.asVector[2] = vec[2];
    
    self->cells.push(cell);
    
    Py_RETURN_NONE;
}

static PyObject *
bitbuf__BitBuf__write_bool(bitbuf__BitBuf *self, PyObject *args)
{
    bool bit;
    if (!PyArg_ParseTuple(args, "b", &bit))
        return NULL;
    
    BitBufCell *cell = new BitBufCell();
    cell->type = BitBufType_Bool;
    cell->data.asBool = bit;
    
    self->cells.push(cell);
    
    Py_RETURN_NONE;
}

static PyObject *
bitbuf__BitBuf__write_byte(bitbuf__BitBuf *self, PyObject *args)
{
    char byte;
    if (!PyArg_ParseTuple(args, "B", &byte))
        return NULL;
    
    BitBufCell *cell = new BitBufCell();
    cell->type = BitBufType_Byte;
    cell->data.asInt = byte;
    
    self->cells.push(cell);
    
    Py_RETURN_NONE;
}

static PyObject *
bitbuf__BitBuf__write_char(bitbuf__BitBuf *self, PyObject *args)
{
    char byte;
    if (!PyArg_ParseTuple(args, "c", &byte))
        return NULL;
    
    BitBufCell *cell = new BitBufCell();
    cell->type = BitBufType_Char;
    cell->data.asInt = byte;
    
    self->cells.push(cell);
    
    Py_RETURN_NONE;
}

static PyObject *
bitbuf__BitBuf__write_coord(bitbuf__BitBuf *self, PyObject *args)
{
    float coord;
    if (!PyArg_ParseTuple(args, "f", &coord))
        return NULL;
    
    BitBufCell *cell = new BitBufCell();
    cell->type = BitBufType_Coord;
    cell->data.asFloat = coord;
    
    self->cells.push(cell);
    
    Py_RETURN_NONE;
}

static PyObject *
bitbuf__BitBuf__write_entity(bitbuf__BitBuf *self, PyObject *args)
{
    PyObject *py_ent;
    int ent_index;
    
    if (!PyArg_ParseTuple(args, "O", &py_ent))
        return NULL;
    
    if (PyObject_IsInstance(py_ent, (PyObject *)&entity__EntityType))
        ent_index = gamehelpers->IndexOfEdict(((entity__Entity *)py_ent)->edict);
    else if (PyInt_Check(py_ent))
        ent_index = PyInt_AS_LONG(py_ent);
    else
    {
        PyErr_Format(_PyExc_TypeError, "`entity` is the wrong type (expected"
            " sourcemod.entity.Entity or int, got \"%s\")",
            py_ent->ob_type->tp_name);
        return NULL;
    }
    
    if (ent_index == -1)
    {
        PyErr_SetString(g_pViperException, "entity is invalid.");
        return NULL;
    }
    
    BitBufCell *cell = new BitBufCell();
    cell->type = BitBufType_Entity;
    cell->data.asInt = ent_index;
    
    self->cells.push(cell);
    
    Py_RETURN_NONE;
}

static PyObject *
bitbuf__BitBuf__write_float(bitbuf__BitBuf *self, PyObject *args)
{
    float num;
    if (!PyArg_ParseTuple(args, "f", &num))
        return NULL;
    
    BitBufCell *cell = new BitBufCell();
    cell->type = BitBufType_Float;
    cell->data.asFloat = num;
    
    self->cells.push(cell);
    
    Py_RETURN_NONE;
}

static PyObject *
bitbuf__BitBuf__write_num(bitbuf__BitBuf *self, PyObject *args)
{
    int num;
    if (!PyArg_ParseTuple(args, "i", &num))
        return NULL;
    
    BitBufCell *cell = new BitBufCell();
    cell->type = BitBufType_Num;
    cell->data.asInt = num;
    
    self->cells.push(cell);
    
    Py_RETURN_NONE;
}

static PyObject *
bitbuf__BitBuf__write_short(bitbuf__BitBuf *self, PyObject *args)
{
    short num;
    if (!PyArg_ParseTuple(args, "h", &num))
        return NULL;
    
    BitBufCell *cell = new BitBufCell();
    cell->type = BitBufType_Short;
    cell->data.asInt = num;
    
    self->cells.push(cell);
    
    Py_RETURN_NONE;
}

static PyObject *
bitbuf__BitBuf__write_string(bitbuf__BitBuf *self, PyObject *args)
{
    char *str;
    if (!PyArg_ParseTuple(args, "s", &str))
        return NULL;
    
    BitBufCell *cell = new BitBufCell();
    cell->type = BitBufType_String;
    cell->data.asString = sm_strdup(str);
    
    self->cells.push(cell);
    
    Py_RETURN_NONE;
}

static PyObject *
bitbuf__BitBuf__write_vec_coord(bitbuf__BitBuf *self, PyObject *args)
{
    PyObject *py_vec;
    float vec[3] = {0.0, 0.0, 0.0};
    
    if (!PyArg_ParseTuple(args, "O", &py_vec))
        return NULL;
    
    if (!PyObjectToVector(py_vec, vec))
        return NULL;
    
    BitBufCell *cell = new BitBufCell();
    cell->type = BitBufType_VecCoord;
    cell->data.asVector[0] = vec[0];
    cell->data.asVector[1] = vec[1];
    cell->data.asVector[2] = vec[2];
    
    self->cells.push(cell);
    
    Py_RETURN_NONE;
}

static PyObject *
bitbuf__BitBuf__write_vec_normal(bitbuf__BitBuf *self, PyObject *args)
{
    PyObject *py_vec;
    float vec[3] = {0.0, 0.0, 0.0};
    
    if (!PyArg_ParseTuple(args, "O", &py_vec))
        return NULL;
    
    if (!PyObjectToVector(py_vec, vec))
        return NULL;
    
    BitBufCell *cell = new BitBufCell();
    cell->type = BitBufType_VecNormal;
    cell->data.asVector[0] = vec[0];
    cell->data.asVector[1] = vec[1];
    cell->data.asVector[2] = vec[2];
    
    self->cells.push(cell);
    
    Py_RETURN_NONE;
}

static PyObject *
bitbuf__BitBuf__write_word(bitbuf__BitBuf *self, PyObject *args)
{
    short word;
    if (!PyArg_ParseTuple(args, "H", &word))
        return NULL;
    
    BitBufCell *cell = new BitBufCell();
    cell->type = BitBufType_Word;
    cell->data.asInt = word;
    
    self->cells.push(cell);
    
    Py_RETURN_NONE;
}


/* Magic methods */
static PyObject *
bitbuf__BitBuf__str__(bitbuf__BitBuf *self)
{
    return PyString_FromFormat("<BitBuf object at: %p>", self);
}

static void
bitbuf__BitBuf__del__(bitbuf__BitBuf *self)
{
    SourceHook::CStack<BitBufCell *>::iterator iter = self->cells.begin();
    while (iter != self->cells.end())
    {
        BitBufCell *cell = (*iter);
        if (cell->type == BitBufType_String)
            delete [] cell->data.asString;
     
        iter++;   
        delete cell;
    }
}

PyMethodDef bitbuf__BitBuf__methods[] = {
    {"write_angle", (PyCFunction)bitbuf__BitBuf__write_angle, METH_VARARGS|METH_KEYWORDS,
        "write_angle(angle[, numBits=8])\n\n"
        "Writes a bit angle to a writable bitbuffer (bf_write).\n"
        "@note: nothing is actually written to a bitbuffer until necessary.\n"
        "@type  angle: float\n"
        "@param angle: Angle to write to the bitbuffer.\n"
        "@type  numBits: int"
        "@param numBits: Optional number of bits to use.\n"},
    {"write_angles", (PyCFunction)bitbuf__BitBuf__write_angles, METH_VARARGS,
        "write_angles(angles)\n\n"
        "Writes a 3D angle vector to a writable bitbuffer (bf_write).\n"
        "@note: nothing is actually written to a bitbuffer until necessary.\n"
        "@type  angles: datatypes.Vector or list-like[3]\n"
        "@param angles: Angles to write to the bitbuffer.\n"},
    {"write_bool", (PyCFunction)bitbuf__BitBuf__write_bool, METH_VARARGS,
        "write_bool(bit)\n\n"
        "Writes a single bit to a writable bitbuffer (bf_write).\n"
        "@note: nothing is actually written to a bitbuffer until necessary.\n"
        "@type  bit: bool\n"
        "@param bit: A single bit to write to the bitbuffer."},
    {"write_byte", (PyCFunction)bitbuf__BitBuf__write_byte, METH_VARARGS,
        "write_byte(byte)\n\n"
        "Writes a byte to a writable bitbuffer (bf_write).\n"
        "@note: nothing is actually written to a bitbuffer until necessary.\n"
        "@type  byte: int\n"
        "@param byte: A byte to write to the bitbuffer. Value will be written as 8bit."},
    {"write_coord", (PyCFunction)bitbuf__BitBuf__write_coord, METH_VARARGS,
        "write_coord(coord)\n\n"
        "Writes a coordinate to a writable bitbuffer (bf_write).\n"
        "@note: nothing is actually written to a bitbuffer until necessary.\n"
        "@type  coord: float\n"
        "@param coord: Coordinate to write to the bitbuffer."},
    {"write_char", (PyCFunction)bitbuf__BitBuf__write_char, METH_VARARGS,
        "write_char(char)\n\n"
        "Writes a character to a writable bitbuffer (bf_write).\n"
        "@note: nothing is actually written to a bitbuffer until necessary.\n"
        "@type  char: str[1]\n"
        "@param char: A character to write to the bitbuffer."},
    {"write_entity", (PyCFunction)bitbuf__BitBuf__write_entity, METH_VARARGS,
        "write_entity(entity)\n\n"
        "Writes an entity to a writable bitbuffer (bf_write).\n"
        "@note: nothing is actually written to a bitbuffer until necessary.\n"
        "@type  entity: entity.Entity or int\n"
        "@param entity: Entity object or entity index to write to the bitbuffer."},
    {"write_float", (PyCFunction)bitbuf__BitBuf__write_float, METH_VARARGS,
        "write_float(num)\n\n"
        "Writes a floating point number to a writable bitbuffer (bf_write).\n"
        "@note: nothing is actually written to a bitbuffer until necessary.\n"
        "@type  num: float\n"
        "@param num: A floating point number to write to the bitbuffer."},
    {"write_num", (PyCFunction)bitbuf__BitBuf__write_num, METH_VARARGS,
        "write_num(num)\n\n"
        "Writes a normal integer to a writable bitbuffer (bf_write).\n"
        "@note: nothing is actually written to a bitbuffer until necessary.\n"
        "@type  num: int\n"
        "@param num: An integer to write to the bitbuffer."},
    {"write_short", (PyCFunction)bitbuf__BitBuf__write_short, METH_VARARGS,
        "write_short(short)\n\n"
        "Writes a 16bit integer to a writable bitbuffer (bf_write).\n"
        "@note: nothing is actually written to a bitbuffer until necessary.\n"
        "@type  short: int\n"
        "@param short: A 16bit integer to write to the bitbuffer."},
    {"write_string", (PyCFunction)bitbuf__BitBuf__write_string, METH_VARARGS,
        "write_string(string)\n\n"
        "Writes a string to a writable bitbuffer (bf_write).\n"
        "@note: nothing is actually written to a bitbuffer until necessary.\n"
        "@type  string: str\n"
        "@param string: Text string to write to the bitbuffer."},
    {"write_vec_coord", (PyCFunction)bitbuf__BitBuf__write_vec_coord, METH_VARARGS,
        "write_vec_coord(vec)\n\n"
        "Writes a 3D vector of coordinates to a writable bitbuffer (bf_write).\n"
        "@note: nothing is actually written to a bitbuffer until necessary.\n"
        "@type  vec: datatypes.Vector or list-like[3]\n"
        "@param vec: A Vector object or list-like object of 3 floats to write to the \n"
        "    bitbuffer."},
    {"write_vec_normal", (PyCFunction)bitbuf__BitBuf__write_vec_normal, METH_VARARGS,
        "write_vec_normal(vec)\n\n"
        "Writes a 3D normal vector to a writable bitbuffer (bf_write).\n"
        "@note: nothing is actually written to a bitbuffer until necessary.\n"
        "@type  vec: datatypes.Vector or list-like[3]\n"
        "@param vec: A Vector object or list-like object of 3 floats to write to the \n"
        "    bitbuffer."},
    {"write_word", (PyCFunction)bitbuf__BitBuf__write_word, METH_VARARGS,
        "write_word(word)\n\n"
        "Writes a 16bit unsigned integer to a writable bitbuffer (bf_write).\n"
        "@note: nothing is actually written to a bitbuffer until necessary.\n"
        "@type  word: int\n"
        "@param word: A 16bit unsigned integer to write to the bitbuffer."},
    {NULL, NULL, 0, NULL}
};

PyTypeObject bitbuf__BitBufType = {
    PyObject_HEAD_INIT(_PyType_Type)
    0,                                              /*ob_size*/
    "sourcemod.bitbuf.BitBuf",                      /*tp_name*/
    sizeof(bitbuf__BitBuf),                         /*tp_basicsize*/
    0,                                              /*tp_itemsize*/
    (destructor)bitbuf__BitBuf__del__,              /*tp_dealloc*/
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
    "Interact with writable Source BitBuffers (bf_write).",
    0,                                              /* tp_traverse */
    0,                                              /* tp_clear */
    0,                                              /* tp_richcompare */
    0,                                              /* tp_weaklistoffset */
    0,                                              /* tp_iter */
    0,                                              /* tp_iternext */
    bitbuf__BitBuf__methods,                        /* tp_methods */
    0,                                              /* tp_members */
    0,                                              /* tp_getset */
    0,                                              /* tp_base */
    0,                                              /* tp_dict */
    0,                                              /* tp_descr_get */
    0,                                              /* tp_descr_set */
    0,                                              /* tp_dictoffset */
    0,                                              /* tp_init */
    0,                                              /* tp_alloc */
    &PyType_GenericNew,                             /* tp_new */
};


/* Methods */
static PyObject *
bitbuf__BitBufRead__read_angle(bitbuf__BitBufRead *self, PyObject *args,
                               PyObject *kwds)
{
    int numBits = 8;
    
    static char *keywdlist[] = {"numBits", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|i", keywdlist, &numBits))
        return NULL;
    
    return PyFloat_FromDouble(self->bf->ReadBitAngle(numBits));
}

static PyObject *
bitbuf__BitBufRead__read_angles(bitbuf__BitBufRead *self)
{
    QAngle fa;
    self->bf->ReadBitAngles(fa);
    
    return CreatePyVector(fa);
}

static PyObject *
bitbuf__BitBufRead__read_bool(bitbuf__BitBufRead *self)
{
    return PyBool_FromLong(self->bf->ReadOneBit());
}

static PyObject *
bitbuf__BitBufRead__read_byte(bitbuf__BitBufRead *self)
{
    return PyInt_FromLong(self->bf->ReadByte());
}

static PyObject *
bitbuf__BitBufRead__read_char(bitbuf__BitBufRead *self)
{
    return PyString_FromFormat("%c", self->bf->ReadChar());
}

static PyObject *
bitbuf__BitBufRead__read_coord(bitbuf__BitBufRead *self)
{
    return PyFloat_FromDouble(self->bf->ReadBitCoord());
}

static PyObject *
bitbuf__BitBufRead__read_entity(bitbuf__BitBufRead *self)
{
    int index = self->bf->ReadShort();
    
    edict_t *pStoredEdict = gamehelpers->EdictOfIndex(index);
    CBaseEntity *pStoredEntity = GetEntity(pStoredEdict);
    if (pStoredEdict == NULL || pStoredEntity == NULL)
    {
        PyErr_Format(g_pViperException, "Entity index %d from bitbuffer is"
            " invalid", index);
        return NULL;
    }

    IServerEntity *pSE = pStoredEdict->GetIServerEntity();
    if (pSE == NULL)
    {
        PyErr_Format(g_pViperException, "Entity index %d from bitbuffer is"
            " invalid", index);
        return NULL;
    }
    
    entity__Entity *pyEntity = (entity__Entity *)entity__EntityType.tp_new(
        &entity__EntityType, NULL, NULL);
    pyEntity->edict = pStoredEdict;
    
    return (PyObject *)pyEntity;
}

static PyObject *
bitbuf__BitBufRead__read_float(bitbuf__BitBufRead *self)
{
    return PyFloat_FromDouble(self->bf->ReadFloat());
}

static PyObject *
bitbuf__BitBufRead__read_num(bitbuf__BitBufRead *self)
{
    return PyInt_FromLong(self->bf->ReadLong());
}

static PyObject *
bitbuf__BitBufRead__read_short(bitbuf__BitBufRead *self)
{
    return PyInt_FromLong(self->bf->ReadShort());
}

static PyObject *
bitbuf__BitBufRead__read_string(bitbuf__BitBufRead *self, PyObject *args,
                                PyObject *kwds)
{
    static char buffer[2048];
    bool bLines = false;
    
    static char *keywdlist[] = {"lines", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|b", keywdlist, &bLines))
        return NULL;
    
    self->bf->ReadString((char *)&buffer, sizeof(buffer), bLines);
    
    return PyString_FromString((char *)&buffer);
}

static PyObject *
bitbuf__BitBufRead__read_word(bitbuf__BitBufRead *self)
{
    return PyInt_FromLong(self->bf->ReadWord());
}

static PyObject *
bitbuf__BitBufRead__read_vec_coord(bitbuf__BitBufRead *self)
{
    Vector fa;
    self->bf->ReadBitVec3Coord(fa);
    
    return CreatePyVector(fa);
}

static PyObject *
bitbuf__BitBufRead__read_vec_normal(bitbuf__BitBufRead *self)
{
    Vector fa;
    self->bf->ReadBitVec3Normal(fa);
    
    return CreatePyVector(fa);
}

/* Magic methods */
static PyObject *
bitbuf__BitBufRead__str__(bitbuf__BitBuf *self)
{
    return PyString_FromFormat("<BitBuf object at: %p>", self);
}

/* Get/Sets */
static PyObject *
bitbuf__BitBufRead__num_bytes_leftget(bitbuf__BitBufRead *self)
{
    if (self->bf == NULL)
    {
        PyErr_SetString(g_pViperException, "Invalid bitbuffer (bf_read *).");
        return NULL;
    }
    
    return PyInt_FromLong(self->bf->GetNumBitsLeft() >> 3);
}

PyMethodDef bitbuf__BitBufRead__methods[] = {
#if NOT_DOCUMENTED_YET
#endif
    {"read_angle", (PyCFunction)bitbuf__BitBufRead__read_angle, METH_KEYWORDS,
        "read_angle([numBits=8]) -> float\n\n"
        "Reads a bit angle from the readable bitbuffer.\n"
        "@type  numBits: int\n"
        "@param numBits: Optional number of bits to use.\n"
        "@rtype: float\n"
        "@return: A bit angle"},
    {"read_angles", (PyCFunction)bitbuf__BitBufRead__read_angles, METH_NOARGS,
        "read_angles() -> datatypes.Vector\n\n"
        "Reads a 3D angle vector from the readable bitbuffer.\n"
        "@rtype: datatypes.Vector\n"
        "@return: A datatypes.Vector object representing a 3D angle vector."},
    {"read_bool", (PyCFunction)bitbuf__BitBufRead__read_bool, METH_NOARGS,
        "read_bool() -> bool\n\n"
        "Reads a single bit from the readable bitbuffer.\n"
        "@rtype: bool\n"
        "@return: A single bit from the bitbuffer."},
    {"read_byte", (PyCFunction)bitbuf__BitBufRead__read_byte, METH_NOARGS,
        "read_byte() -> int\n\n"
        "Reads a byte from the readable bitbuffer.\n"
        "@rtype: int\n"
        "@return: The byte read from the bitbuffer. Only the first 8 bits are used."},
    {"read_char", (PyCFunction)bitbuf__BitBufRead__read_char, METH_NOARGS,
        "read_char() -> str[1]\n\n"
        "Reads a character from the readable bitbuffer.\n"
        "@rtype: str[1]\n"
        "@return: A string of length 1 containing the character read."},
    {"read_coord", (PyCFunction)bitbuf__BitBufRead__read_coord, METH_NOARGS,
        "read_coord() -> float\n\n"
        "Reads a coordinate from the readable bitbuffer.\n"
        "@rtype: float\n"
        "@return: The coordinate read from the bitbuffer."},
    {"read_entity", (PyCFunction)bitbuf__BitBufRead__read_entity, METH_NOARGS,
        "read_entity() -> entity.Entity\n\n"
        "Reads an entity from the readable bitbuffer.\n"
        "@rtype: entity.Entity\n"
        "@return: An entity.Entity object representing an entity read from the bitbuffer."},
    {"read_float", (PyCFunction)bitbuf__BitBufRead__read_float, METH_NOARGS,
        "read_float() -> float\n\n"
        "Reads a floating point number from the readable bitbuffer.\n"
        "@rtype: float\n"
        "@return: The floating point number read from the bitbuffer."},
    {"read_num", (PyCFunction)bitbuf__BitBufRead__read_num, METH_NOARGS,
        "read_num() -> int\n\n"
        "Reads a normal integer from the readable bitbuffer.\n"
        "@rtype: int\n"
        "@return: The 32-bit integer value read from the bitbuffer."},
    {"read_short", (PyCFunction)bitbuf__BitBufRead__read_short, METH_NOARGS,
        "read_short() -> int\n\n"
        "Reads a 16-bit integer from the readable bitbuffer.\n"
        "@rtype: int\n"
        "@return: The 16-bit integer value read from the bitbuffer."},
    {"read_string", (PyCFunction)bitbuf__BitBufRead__read_string, METH_KEYWORDS,
        "read_string([lines=False]) -> str\n\n"
        "Reads a string from the readable bitbuffer.\n"
        "@type  lines: bool\n"
        "@param lines: If True the buffer will be read until it reaches a newline or\n"
        "    null terminator."
        "@rtype: str\n"
        "@return: The string value read from the bitbuffer."},
    {"read_word", (PyCFunction)bitbuf__BitBufRead__read_word, METH_NOARGS,
        "read_short() -> int\n\n"
        "Reads a 16-bit unsigned integer from the readable bitbuffer.\n"
        "@rtype: int\n"
        "@return: The 16-bit unsigned integer value read from the bitbuffer."},
    {"read_vec_coord", (PyCFunction)bitbuf__BitBufRead__read_vec_coord, METH_NOARGS,
        "read_vec_coord() -> datatypes.Vector\n\n"
        "Reads a 3D vector of coordinates from the readable bitbuffer.\n"
        "@rtype: datatypes.Vector\n"
        "@return: A datatypes.Vector object representing a 3D vector of coordinates."},
    {"read_vec_normal", (PyCFunction)bitbuf__BitBufRead__read_vec_normal, METH_NOARGS,
        "read_vec_coord() -> datatypes.Vector\n\n"
        "Reads a 3D normal vector from the readable bitbuffer.\n"
        "@rtype: datatypes.Vector\n"
        "@return: A datatypes.Vector object representing a 3D normal vector."},
    {NULL, NULL, 0, NULL}
};

PyGetSetDef bitbuf__BitBufRead__getsets[] = {
    {"num_bytes_left", (getter)bitbuf__BitBufRead__num_bytes_leftget, NULL,
        "The number of bytes left in the bf_read."},
    {NULL}
};

PyTypeObject bitbuf__BitBufReadType = {
    PyObject_HEAD_INIT(_PyType_Type)
    0,                                              /*ob_size*/
    "sourcemod.bitbuf.BitBufRead",                  /*tp_name*/
    sizeof(bitbuf__BitBufRead),                     /*tp_basicsize*/
    0,                                              /*tp_itemsize*/
    0,                                              /*tp_dealloc*/
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
    (reprfunc)bitbuf__BitBufRead__str__,            /*tp_str*/
    0,                                              /*tp_getattro*/
    0,                                              /*tp_setattro*/
    0,                                              /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE,         /*tp_flags*/
    /* tp_doc */
    "Interact with readable Source BitBuffers (bf_read).",
    0,                                              /* tp_traverse */
    0,                                              /* tp_clear */
    0,                                              /* tp_richcompare */
    0,                                              /* tp_weaklistoffset */
    0,                                              /* tp_iter */
    0,                                              /* tp_iternext */
    bitbuf__BitBufRead__methods,                    /* tp_methods */
    0,                                              /* tp_members */
    bitbuf__BitBufRead__getsets,                    /* tp_getset */
    0,                                              /* tp_base */
    0,                                              /* tp_dict */
    0,                                              /* tp_descr_get */
    0,                                              /* tp_descr_set */
    0,                                              /* tp_dictoffset */
    0,                                              /* tp_init */
    0,                                              /* tp_alloc */
    &PyType_GenericNew,                             /* tp_new */
};

PyObject *
initbitbuf(void)
{
    Py_INCREF(_PyType_Type);
    bitbuf__BitBufType.ob_type = _PyType_Type;
    
    Py_INCREF(_PyType_Type);
    bitbuf__BitBufReadType.ob_type = _PyType_Type;
    
    if (PyType_Ready(&bitbuf__BitBufType) < 0 ||
        PyType_Ready(&bitbuf__BitBufReadType) < 0)
        return NULL;
    
    PyObject *bitbuf = Py_InitModule3("bitbuf", NULL,
        "Manipulation of Source bit buffers");
    
    Py_INCREF((PyObject *)&bitbuf__BitBufType);
    PyModule_AddObject(bitbuf, "BitBuf", (PyObject *)&bitbuf__BitBufType);
    
    Py_INCREF((PyObject *)&bitbuf__BitBufReadType);
    PyModule_AddObject(bitbuf, "BitBufRead", (PyObject *)&bitbuf__BitBufReadType);
    
    return bitbuf;
}

PyObject *
GetBitBuf()
{
    return bitbuf__BitBufType.tp_new(&bitbuf__BitBufType,
        NULL, NULL);
}

bool
BitBufToBfWrite(bitbuf__BitBuf *obj, bf_write *bitbuf)
{
    if (bitbuf == NULL)
        return false;
    
    SourceHook::CStack<BitBufCell *>::iterator iter;
    for (iter=obj->cells.begin(); iter!=obj->cells.end(); iter++)
    {
        BitBufCell *cell = (*iter);
        switch (cell->type)
        {
        case BitBufType_Bool:
            bitbuf->WriteOneBit(cell->data.asInt);
            break;
        
        case BitBufType_Byte:
            bitbuf->WriteByte(cell->data.asInt);
            break;
        
        case BitBufType_Char:
            bitbuf->WriteChar(cell->data.asInt);
            break;
        
        case BitBufType_Short:
            bitbuf->WriteChar(cell->data.asInt);
            break;
        
        case BitBufType_Word:
            bitbuf->WriteWord(cell->data.asInt);
            break;
        
        case BitBufType_Num:
            bitbuf->WriteLong(cell->data.asLong);
            break;
        
        case BitBufType_Float:
            bitbuf->WriteFloat(cell->data.asFloat);
            break;
        
        case BitBufType_String:
            bitbuf->WriteString(cell->data.asString);
            break;
        
        case BitBufType_Entity:
            bitbuf->WriteShort(cell->data.asInt);
            break;
        
        case BitBufType_Angle:
            bitbuf->WriteBitAngle(cell->data.asAngle.value,
                cell->data.asAngle.numbits);
            break;
        
        case BitBufType_Coord:
            bitbuf->WriteBitCoord(cell->data.asFloat);
            break;
        
        case BitBufType_VecCoord:
            {
                Vector vec(cell->data.asVector[0], cell->data.asVector[1],
                    cell->data.asVector[2]);
                bitbuf->WriteBitVec3Coord(vec);
                break;
            }
        
        case BitBufType_VecNormal:
            {
                Vector vec(cell->data.asVector[0], cell->data.asVector[1],
                    cell->data.asVector[2]);
                bitbuf->WriteBitVec3Normal(vec);
                break;
            }
        
        case BitBufType_Angles:
            {
                QAngle ang(cell->data.asVector[0], cell->data.asVector[1],
                    cell->data.asVector[2]);
                bitbuf->WriteBitAngles(ang);
                break;
            }
        };
    }
    
    return true;
}

