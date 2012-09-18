#ifndef __INCLUDE_NETSTATSTYPE_H__
#define __INCLUDE_NETSTATSTYPE_H__

#include "Macros.h"
#include "HL2SDK.h"

class NetStatsType {
public:
	NetStatsType(float in, float out);

	float In;
	float Out;
};

#endif