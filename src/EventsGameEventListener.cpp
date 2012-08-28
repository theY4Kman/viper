#include "BoostPythonSM.h"
#include "EventsModule.h"
#include "sdk/smsdk_ext.h"
#include "Macros.h"
#include "ViperExtension.h"
#include "EventsGameEventListener.h"

namespace py = boost::python;

EventsGameEventListener events__GameEventListener;

EventsGameEventListener::EventsGameEventListener() {

}

void EventsGameEventListener::FireGameEvent(IGameEvent *gameEvent) {

}