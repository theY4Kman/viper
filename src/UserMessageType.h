#ifndef __INCLUDE_USERMESSAGETYPE_H__
#define __INCLUDE_USERMESSAGETYPE_H__

#include "Macros.h"
#include "ViperExtension.h"
#include "HL2SDK.h"

class UserMessageType {
public:
	UserMessageType(int id, std::string name);

	BitBufType Start(boost::python::list clientsList, bool reliable = false, bool initMessage = false);
	void End();

public:
	int ID;
	std::string Name;
};

#endif