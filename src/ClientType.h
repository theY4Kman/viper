#ifndef __INCLUDE_CLIENTTYPE_H__
#define __INCLUDE_CLIENTTYPE_H__

#include "BoostPythonSM.h"
#include "Macros.h"
#include "STL.h"
#include "EntityType.h"
#include "EdictType.h"

class ClientType {
	public:
		ClientType(int index);

		EntityType GetEntity();
		EdictType GetEdict();
		std::string GetName();
		bool IsConnected();
		bool IsInGame();

		std::string StrMagic();

	private:
		int Index;
};

#endif