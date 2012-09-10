#include "EventType.h"
#include "Macros.h"
#include "BoostPythonSM.h"
#include "Util.h"
#include "InvalidEventExceptionType.h"
#include "EventNoLongerValidExceptionType.h"
#include "EventsModule.h"
#include "InterfaceContainer.h"

namespace py = boost::python;

EventType::EventType(std::string name, IGameEvent *gameEvent) {
	if(gameEvent == NULL) {
		throw InvalidEventExceptionType(name);
	}

	Name = name;
	Event = gameEvent;
}

void EventType::Cancel() {
	if(Event == NULL) {
		throw EventNoLongerValidExceptionType(this);
	}

	events__CanceledEvents.push_back(Event);
	g_Interfaces.GameEventManagerInstance->FreeEvent(Event);
    
	Event = NULL;
}

void EventType::Fire(bool dontBroadcast) {
	if(Event == NULL) {
		throw EventNoLongerValidExceptionType(this);
	}

	g_Interfaces.GameEventManagerInstance->FireEvent(Event, dontBroadcast);
    
	Event = NULL;
}

boost::python::dict EventType::GetFields() {
	py::dict fieldsDict;

	for(std::vector<EventFieldType>::iterator it = events__ModEvents[Name].begin(); it != events__ModEvents[Name].end(); it++) {
		EventFieldType eventField = *it;

		std::string typeStr = EventFieldTypeStrings[eventField.FieldType];

		fieldsDict[eventField.Name.c_str()] = typeStr;
	}

	return fieldsDict;
}

bool EventType::IsEmpty(std::string field) {
	if(Event == NULL) {
		throw EventNoLongerValidExceptionType(this);
	}

	return Event->IsEmpty(field.c_str());
}

std::string EventType::GetName() {
	return Name;
}

bool EventType::IsReliable() {
	if(Event == NULL) {
		throw EventNoLongerValidExceptionType(this);
	}

	return Event->IsReliable();
}

bool EventType::IsLocal() {
	if(Event == NULL) {
		throw EventNoLongerValidExceptionType(this);
	}

	return Event->IsLocal();
}

bool EventType::GetBool(std::string fieldName, bool defaultValue) {
	if(Event == NULL) {
		throw EventNoLongerValidExceptionType(this);
	}

	return Event->GetBool(fieldName.c_str(), defaultValue);
}

int EventType::GetInt(std::string fieldName, int defaultValue) {
	if(Event == NULL) {
		throw EventNoLongerValidExceptionType(this);
	}

	return Event->GetInt(fieldName.c_str(), defaultValue);
}

float EventType::GetFloat(std::string fieldName, float defaultValue) {
	if(Event == NULL) {
		throw EventNoLongerValidExceptionType(this);
	}

	return Event->GetFloat(fieldName.c_str(), defaultValue);
}

std::string EventType::GetString(std::string fieldName, std::string defaultValue) {
	if(Event == NULL) {
		throw EventNoLongerValidExceptionType(this);
	}

	return std::string(Event->GetString(fieldName.c_str(), defaultValue.c_str()));
}

void EventType::SetBool(std::string fieldName, bool value) {
	if(Event == NULL) {
		throw EventNoLongerValidExceptionType(this);
	}

	Event->SetBool(fieldName.c_str(), value);
}

void EventType::SetInt(std::string fieldName, int value) {
	if(Event == NULL) {
		throw EventNoLongerValidExceptionType(this);
	}

	Event->SetInt(fieldName.c_str(), value);
}

void EventType::SetFloat(std::string fieldName, float value) {
	if(Event == NULL) {
		throw EventNoLongerValidExceptionType(this);
	}

	Event->SetFloat(fieldName.c_str(), value);
}

void EventType::SetString(std::string fieldName, std::string value) {
	if(Event == NULL) {
		throw EventNoLongerValidExceptionType(this);
	}

	Event->SetString(fieldName.c_str(), value.c_str());
}