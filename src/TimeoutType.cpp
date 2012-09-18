#include "TimeoutType.h"
#include "TimersModule.h"
#include "InvalidTimeoutExceptionType.h"
#include "Util.h"

namespace py = boost::python;

TimeoutType::TimeoutType(TimeoutInfo *info) {
	Info = info;
}

void TimeoutType::Cancel() {
	if(!timers__IsTimeoutValid(Info)) {
		throw InvalidTimeoutExceptionType();
	}

	Info->Cancel();
}

void TimeoutType::Execute() {
	if(!timers__IsTimeoutValid(Info)) {
		throw InvalidTimeoutExceptionType();
	}

	Info->Execute();
}

void TimeoutType::Reset() {
	if(!timers__IsTimeoutValid(Info)) {
		throw InvalidTimeoutExceptionType();
	}

	Info->Reset();
}