#include "IntervalType.h"
#include "TimersModule.h"
#include "InvalidIntervalExceptionType.h"
#include "Util.h"

namespace py = boost::python;

IntervalType::IntervalType(IntervalInfo *info) {
	Info = info;
}

void IntervalType::Cancel() {
	if(!timers__IsIntervalValid(Info)) {
		throw InvalidIntervalExceptionType();
	}

	Info->Cancel();
}

void IntervalType::Execute(bool reset) {
	if(!timers__IsIntervalValid(Info)) {
		throw InvalidIntervalExceptionType();
	}

	Info->Execute();

	if(reset) {
		Info->Reset();
	}
}

void IntervalType::Reset() {
	if(!timers__IsIntervalValid(Info)) {
		throw InvalidIntervalExceptionType();
	}

	Info->Reset();
}