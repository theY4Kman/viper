/**
 * =============================================================================
 * Viper
 * Copyright (C) 2008-2009 Zach "theY4Kman" Kanzler
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

#ifndef _INCLUDE_VIPER_IFORWARDSYS_H_
#define _INCLUDE_VIPER_IFORWARDSYS_H_

#include <Python.h>
#include <IViperPluginSys.h>

enum ViperResultType
{
     Pl_Continue = 0,   /**< No result */
     Pl_Changed = 1,    /**< Inputs or outputs have been overridden with new values */
     Pl_Handled = 3,    /**< Result was handled, stop at the end */
     Pl_Stop = 4,       /**< Result was handled, stop now */
};

enum ViperExecType
{
    ET_Ignore = 0,  /**< Ignore all return values, return NULL */
    ET_Single = 1,  /**< Only return the last exec, ignore all others */
    ET_Event = 2,   /**< Acts as an event with the ResultTypes above, no mid-Stops allowed, returns highest if number */
    ET_Hook = 3,    /**< Acts as a hook with the ResultTypes above, mid-Stops allowed, returns highest if number */
    ET_LowEvent = 4,/**< Same as ET_Event, except that it returns the lowest value if number */
};

typedef ViperResultType (*IForwardCallback)(PyObject*, IViperPluginFunction*);

class IViperForward
{
public:
    /**
     * @brief Executes all the callbacks with the given arguments and keywords
     * @param result    A pointer to an int to store the number result
     * @param args  A VALID Python Tuple to use as arguments; must NOT be NULL
     * @return Returns the last exec's result unless ExecType is ET_Ignore, or NULL on error
     */
    virtual PyObject *Execute(int *result, PyObject *args) =0;
    
    /**
     * @brief Returns the name of the forward
     *
     * @return Forward name
     */
    virtual char const *GetForwardName() =0;
    
    /**
     * @brief Adds a function to the forward
     */
    virtual bool AddFunction(IViperPluginFunction *func) =0;
    
    /**
     * @brief Removes a function from the forward
     */
    virtual bool RemoveFunction(IViperPluginFunction *func) =0;
    
    /**
     * @brief Removes all functions owned by a plugin from the forward
     */
    virtual bool RemoveFunctionsOfPlugin(IViperPlugin *plugin) =0;
    
    /**
     * @brief Returns the number of functions hooked on this forward
     *
     * @return Number of functions in forward
     */
    virtual unsigned int GetFunctionCount() =0;
    
    /**
     * @brief Returns the execution type of the forward
     */
    virtual ViperExecType GetExecType() =0;
    
    /**
     * @brief Returns the Python types of the parameters
     */
    virtual PyObject *GetParamTypes() =0;
};

class IViperForwardManager
{
public:
    /**
     * @brief Creates a global forward.
     * The name used to create the forward is used when registering a callback
     * in a target plug-in. As new non-private plug-ins are loaded or unloaded,
     * the callbacks wills will be automatically loaded or unloaded.
     * 
     * @param name  Name of forward
     * @param et    Execution type to be used
     * @param types A Python tuple containing the types of the parameters
     * @param callback  The callback to call when the result of a forward call is reached
     * @return  New IForward on success, NULL otherwise.
     */
    virtual IViperForward *CreateForward(char const *name, ViperExecType et,
                                         PyObject *types,
                                         IForwardCallback callback) =0;
    
    /**
     * @brief Finds a forward by its name.
     * 
     * @return  An IForward pointer on success, or NULL if none were found
     *          matching the name
     */
    virtual IViperForward *FindForward(char const *name) =0;
    
    /**
     * @brief Frees and destroys an IForward object
     *
     * @param forward   An IForward created by CreateForward()
     */
    virtual void ReleaseForward(IViperForward *forward) =0;
};

#endif /* _INCLUDE_VIPER_IFORWARDSYS_H_ */
