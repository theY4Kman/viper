#ifndef __INCLUDE_ENTITYPROPERTYFIELDTYPES_H__
#define __INCLUDE_ENTITYPROPERTYFIELDTYPES_H__

#include "Macros.h"
#include "ViperExtension.h"
#include "HL2SDK.h"

enum EntityPropertyFieldTypes {
	PropField_Unsupported = -1,
	PropField_Integer = 0,
	PropField_Float,
	PropField_Entity,
	PropField_Vector,
	PropField_String,
	PropField_String_T
};

#endif