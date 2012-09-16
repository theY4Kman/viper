#ifndef __INCLUDE_POINTCONTENTSTYPE_H__
#define __INCLUDE_POINTCONTENTSTYPE_H__

class PointContentsType {
public:
	PointContentsType(int entityIndex, int contentsMask);

	int EntityIndex;
	int ContentsMask;
};

#endif