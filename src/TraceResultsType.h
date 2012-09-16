#ifndef __INCLUDE_TRACERESULTSTYPE_H__
#define __INCLUDE_TRACERESULTSTYPE_H__

#include "VectorType.h"
#include "HL2SDK.h"

class TraceResultsType {
public:
	TraceResultsType(float fraction, VectorType endPosition, int entityIndex, bool didHit, int hitGroup, VectorType planeNormal);

	float Fraction;
	VectorType EndPosition;
	int EntityIndex;
	bool DidHit;
	int HitGroup;
	VectorType PlaneNormal;
};

#endif