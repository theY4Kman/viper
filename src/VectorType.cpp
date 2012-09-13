#include "BoostPythonSM.h"
#include "VectorType.h"
#include "sdk/smsdk_ext.h"
#include "Macros.h"
#include "Util.h"

namespace py = boost::python;

VectorType::VectorType(float x, float y, float z) {
	X = x;
	Y = y;
	Z = z;
}

VectorType::VectorType(const VectorType &v) {
	X = v.X;
	Y = v.Y;
	Z = v.Z;
}

bool VectorType::CompareEqual(VectorType v) {
	return X == v.X &&
		Y == v.Y &&
		Z == v.Z;
}

bool VectorType::CompareNotEqual(VectorType v) {
	return !CompareEqual(v);
}

bool VectorType::CompareGreaterThan(VectorType v) {
	return X > v.X &&
		Y > v.Y &&
		Z > v.Z;
}

bool VectorType::CompareLessThan(VectorType v) {
	return X < v.X &&
		Y < v.Y &&
		Z < v.Z;
}

bool VectorType::CompareGreaterThanOrEqual(VectorType v) {
	return X >= v.X &&
		Y >= v.Y &&
		Z >= v.Z;
}

bool VectorType::CompareLessThanOrEqual(VectorType v) {
	return X <= v.X &&
		Y <= v.Y &&
		Z <= v.Z;
}