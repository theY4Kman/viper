#include "viper_globals.h"

namespace Viper {
	PyObject *CreatePyVector(const Vector &vec)
	{
		return CreatePyVector(vec.x, vec.y, vec.z);
	}
	PyObject *CreatePyVector(const QAngle &ang)
	{
		return CreatePyVector(ang.x, ang.y, ang.z);
	}

	Vector *VectorFromPyVector(datatypes__Vector *vec)
	{
		return new Vector(vec->x, vec->y, vec->z);
	}

	PyObject *CreatePyColor(Color color)
	{
		return CreatePyColor(color[0], color[1], color[2], color[3]);
	}

	Color *ColorFromPyColor(datatypes__Color *color)
	{
		return new Color(color->r, color->g, color->b, color->a);
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

	PyObject *
	CreatePyColor(int r, int g, int b, int a)
	{
		datatypes__Color *color = (datatypes__Color *)datatypes__ColorType.tp_new(
			&datatypes__ColorType, NULL, NULL);
    
		color->r = r;
		color->g = g;
		color->b = b;
		color->a = a;
    
		return (PyObject *)color;
	}
}