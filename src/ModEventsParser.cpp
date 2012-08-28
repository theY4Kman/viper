#include "BoostPythonSM.h"
#include "EventsModule.h"
#include "sdk/smsdk_ext.h"
#include "Macros.h"
#include "ViperExtension.h"
#include "ModEventsParser.h"

namespace py = boost::python;

std::map<std::string, std::vector<EventFieldType>> ModEventsParser::Parse()
{
	// Capture all GAME search directories
	char gamePathStr[4096];
	g_Interfaces.BaseFileSystemInstance->GetSearchPath("GAME", false, gamePathStr, sizeof(gamePathStr));

	// Split them up by ;
	std::vector<std::string> gamePaths;
	boost::split(gamePaths, std::string(gamePathStr), boost::is_any_of(";"));

	// Build up a list of event resource files
	std::vector<std::string> eventResourceFiles;
	int gamePathCount = gamePaths.size();

	for(int gamePathIndex = gamePathCount - 1; gamePathIndex >= 0; gamePathIndex--) {
		std::string gamePath = gamePaths[gamePathIndex];

#ifdef WIN32
		eventResourceFiles.push_back(gamePath + "resource\\gameevents.res");
		eventResourceFiles.push_back(gamePath + "resource\\modevents.res");
#else
		eventResourceFiles.push_back(gamePath + "resource/gameevents.res");
		eventResourceFiles.push_back(gamePath + "resource/modevents.res");
#endif
	}

	std::map<std::string, std::vector<EventFieldType>> modEvents;

	for(std::vector<std::string>::iterator it = eventResourceFiles.begin(); it != eventResourceFiles.end(); it++) {
		std::string resourceFile = *it;
    
		KeyValues *kv = new KeyValues("modevents.res");
		if (!kv->LoadFromFile(g_Interfaces.BaseFileSystemInstance, resourceFile.c_str()))
		{
			continue;
		}

		// Loop through events
		for (KeyValues *events=kv->GetFirstSubKey(); events; events=events->GetNextKey())
		{
			// TODO: do we need to destroy this?
			std::vector<EventFieldType> fields;
        
			// Loop through their fields
			for (KeyValues *sub=events->GetFirstSubKey(); sub; sub=sub->GetNextKey())
			{
				EventFieldTypes type;
				std::string typeString(sub->GetString());
				if (typeString == "string")
					type = EventFieldType_String;
				else if (typeString == "bool")
					type = EventFieldType_Bool;
				else if (typeString == "byte")
					type = EventFieldType_Byte;
				else if (typeString == "short")
					type = EventFieldType_Short;
				else if (typeString == "long")
					type = EventFieldType_Long;
				else if (typeString == "float")
					type = EventFieldType_Float;
				else if (typeString == "local")
					type = EventFieldType_Local;
				else
					continue;
			
				EventFieldType field(sub->GetName(), type);	
            
				fields.push_back(field);
			}
        
			modEvents[std::string(events->GetName())] = fields;
		}
	}

	return modEvents;
}