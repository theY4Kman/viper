#ifndef __INCLUDE_TIMEOUTTYPE_H__
#define __INCLUDE_TIMEOUTTYPE_H__

#include "Macros.h"
#include "BoostPythonSM.h"
#include "TimeoutInfo.h"

class TimeoutType {
public:
	TimeoutType(TimeoutInfo *info);

	void Cancel();
	void Execute();
	void Reset();

private:
	TimeoutInfo *Info;
};

#endif