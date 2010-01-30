/**
 * =============================================================================
 * Viper
 * Copyright (C) 2007-2010 Zach "theY4Kman" Kanzler
 * Copyright (C) 2004-2007 AlliedModders LLC.  All rights reserved.
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

/**
 * @file IEngineSpecific.h
 * @brief An interface to functions specific to different engine and MM:S versions
 */

#ifndef _INCLUDE_IENGINESPECIFIC_H_
#define _INCLUDE_IENGINESPECIFIC_H_

class IEngineSpecific
{
public:
    /**
     * Get the name of the current map
     */
    virtual char const *GetCurrentMap(void) = 0;
    
    /**
     * Get the game time based on the tick
     */
    virtual float GetGameTime(void) = 0;
};

#endif /* _INCLUDE_IENGINESPECIFIC_H_ */

