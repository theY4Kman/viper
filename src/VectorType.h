#ifndef __INCLUDE_VECTORTYPE_H__
#define __INCLUDE_VECTORTYPE_H__

class VectorType {
public:
	VectorType(const VectorType &v);
	VectorType(float x = 0.0f, float y = 0.0f, float z = 0.0f);

	int Compare(VectorType v);
	bool CompareEqual(VectorType v);
	bool CompareNotEqual(VectorType v);
	bool CompareGreaterThan(VectorType v);
	bool CompareLessThan(VectorType v);
	bool CompareGreaterThanOrEqual(VectorType v);
	bool CompareLessThanOrEqual(VectorType v);
	std::string ReprMagic();

	float X;
	float Y;
	float Z;
};

#endif