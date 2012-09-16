#include "TraceResultsType.h"
#include "NullReferenceExceptionType.h"
#include "InterfaceContainer.h"

TraceResultsType::TraceResultsType(float fraction, VectorType endPosition, int entityIndex, bool didHit, int hitGroup, VectorType planeNormal) {
	Fraction = fraction;
	EndPosition = endPosition;
	EntityIndex = entityIndex;
	DidHit = didHit;
	HitGroup = hitGroup;
	PlaneNormal = planeNormal;
}