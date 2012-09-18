#ifndef __INCLUDE_INTERVALTYPE_H__
#define __INCLUDE_INTERVALTYPE_H__

#include "Macros.h"
#include "BoostPythonSM.h"
#include "IntervalInfo.h"

class IntervalType {
public:
	IntervalType(IntervalInfo *info);
	
	void Cancel();
	void Execute(bool reset = true);
	void Reset();

private:
	IntervalInfo *Info;
};

#endif