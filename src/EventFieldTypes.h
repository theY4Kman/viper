#ifndef __INCLUDE_EVENTFIELDTYPES_H__
#define __INCLUDE_EVENTFIELDTYPES_H__

enum EventFieldTypes {
	EventFieldType_Unknown = -1,
	EventFieldType_String = 0,
	EventFieldType_Bool,
	EventFieldType_Byte,
	EventFieldType_Short,
	EventFieldType_Long,
	EventFieldType_Float,
	EventFieldType_Local
};

extern char const *EventFieldTypeStrings[];

#endif