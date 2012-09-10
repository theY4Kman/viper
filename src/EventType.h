#ifndef __INCLUDE_EVENTTYPE_H__
#define __INCLUDE_EVENTTYPE_H__

#include "STL.h"
#include "HL2SDK.h"
#include "EventFieldType.h"
#include "BoostPythonSM.h"

class EventType {
public:
	EventType(std::string name, IGameEvent *gameEvent);

	void Cancel();
	void Fire(bool dontBroadcast = false);
	boost::python::dict GetFields();
	bool IsEmpty(std::string field);

	std::string GetName();
	bool IsReliable();
	bool IsLocal();

	bool GetBool(std::string fieldName, bool defaultValue = false );
	int GetInt(std::string fieldName, int defaultValue = 0 );
	float GetFloat(std::string fieldName, float defaultValue = 0.0f);
	std::string GetString(std::string fieldName, std::string defaultValue = std::string());

	void SetBool(std::string fieldName, bool value);
	void SetInt(std::string fieldName, int value);
	void SetFloat(std::string fieldName, float value);
	void SetString(std::string fieldName, std::string value);
private:
	std::string Name;
	IGameEvent *Event;
};

#endif