/**
 * =============================================================================
 * Viper
 * Copyright (C) 2007-2010 Zach "theY4Kman" Kanzler
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

/**
 * !!TODO!!
 * The IViper instance must be exposed to Python so that extensions may utilize
 * it and IViperShareSys
 */

#ifndef _INCLUDE_VIPER_IFACE_SHARESYS_H_
#define _INCLUDE_VIPER_IFACE_SHARESYS_H_

/**
 * @brief Defines the base functionality required by a shared interface.
 */
class ViperInterface
{
public:
    /**
     * @brief Must return an integer defining the interface's version.
     */
    virtual unsigned int GetInterfaceVersion() =0;

    /**
     * @brief Must return a string defining the interface's unique name.
     */
    virtual char const *GetInterfaceName() =0;

    /**
     * @brief Must return whether the requested version number is backwards compatible.
     * Note: This can be overridden for breaking changes or custom versioning.
     * 
     * @param version   Version number to compare against.
     * @return  True if compatible, false otherwise.
     */
    virtual bool IsVersionCompatible(unsigned int version)
    {
        if (version > GetInterfaceVersion())
        {
            return false;
        }

        return true;
    }
};

#endif /* _INCLUDE_VIPER_IFACE_SHARESYS_H_ */
