/**
 * =============================================================================
 * Viper
 * Copyright (C) 2012 PimpinJuice
 * Copyright (C) 2007-2012 Zach "theY4Kman" Kanzler
 * Copyright (C) 2004-2007 AlliedModders LLC.
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _INCLUDE_PYTHON_ENTITY_H_
#define _INCLUDE_PYTHON_ENTITY_H_

#include <iserverunknown.h>
#include <edict.h>

namespace Viper {
	namespace Python {
		struct entity__EntityProps;

		extern PyTypeObject entity__EntityType;
		extern void *g_pSendProxy_EHandleToInt;

		enum ViperPropType
		{
			Prop_Send = 0,
			Prop_Data
		};

		struct entity__Entity {
			PyObject_HEAD
    
			edict_t *edict;
    
			entity__EntityProps *sendprops;
			entity__EntityProps *datamaps;
		};

		PyObject * GetEntityPropPyObject(entity__Entity *pyEnt, char const *prop,
										 ViperPropType type, void *propdata=NULL,
										 int a_prop_offset=-1);

		inline CBaseEntity *GetEntity(edict_t *pEdict)
		{
			IServerUnknown *pUnk = pEdict->GetUnknown();
			if (pUnk == NULL)
				return NULL;
    
			return pUnk->GetBaseEntity();
		}
	}
}

#endif//_INCLUDE_PYTHON_ENTITY_H_

