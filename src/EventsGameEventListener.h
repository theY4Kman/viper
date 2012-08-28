#ifndef __INCLUDE_EVENTSGAMEEVENTLISTENER_H__
#define __INCLUDE_EVENTSGAMEEVENTLISTENER_H__

#include "STL.h"
#include "HL2SDK.h"
#include "Macros.h"

class EventsGameEventListener : public IGameEventListener2 {
public:
	EventsGameEventListener();

public:
	void FireGameEvent(IGameEvent *gameEvent);

public:
	bool OnFireEvent(IGameEvent *gameEvent, bool dontBroadcast);
	bool OnFireEventPost(IGameEvent *gameEvent, bool dontBroadcast);
};

extern EventsGameEventListener events__GameEventListener;

#endif