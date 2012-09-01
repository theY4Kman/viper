#ifndef __INCLUDE_CONSOLEVARIABLETYPE_H__
#define __INCLUDE_CONSOLEVARIABLETYPE_H__

#include "Macros.h"
#include "HL2SDK.h"

class ConsoleVariableType {
public:
	ConsoleVariableType(std::string name, ConVar *conVarInstance, bool isFromViper);

	bool GetBool();
	int GetInt();
	float GetFloat();
	std::string GetString();
	void SetBool(bool value, bool replicate = false, bool notify = false);
	void SetInt(int value, bool replicate = false, bool notify = false);
	void SetFloat(float value, bool replicate = false, bool notify = false);
	void SetString(std::string value, bool replicate = false, bool notify = false);
	void Reset(bool replicate = false, bool notify = false);
	std::string GetDefault();
	int GetFlags();
	void SetFlags(int flags);
	bool HasMinimum();
	bool HasMaximum();
	void SetMinimum(bool hasMinimum, float minimum = 0.0f);
	void SetMaximum(bool hasMaximum, float maximum = 0.0f);
	std::string GetName();
	void SendValueToClient(int clientIndex, std::string value);
	void HookChange(boost::python::object callbackFunction);
	void UnhookChange(boost::python::object callbackFunction);

private:
	std::string Name;
	ConVar *ConVarInstance;
	bool IsFromViper;
};

#endif