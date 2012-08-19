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

#ifndef _INCLUDE_VIPER_IVIPERPLUGINSYS_H_
#define _INCLUDE_VIPER_IVIPERPLUGINSYS_H_

#include <Python.h>
#include "IViperShareSys.h"
#include "IViperForwardSys.h"

namespace Viper
{
	/** 
	 * @brief Encapsulates plugin public information exposed through "myinfo."
	 */
	typedef struct viper_plugininfo_s
	{
		const char *name;			/**< Plugin name */
		const char *author;			/**< Plugin author */
		const char *description;	/**< Plugin description */
		const char *version;		/**< Plugin version string */
		const char *url;			/**< Plugin URL */
	} viper_plugininfo_t;

	enum ViperLoadRes
	{
		ViperLoadRes_Successful,
		ViperLoadRes_AlreadyLoaded,
		ViperLoadRes_Failure,
		ViperLoadRes_NeverLoad
	};

	/**
	 * @brief Describes the usability status of a plugin.
	 */
	enum ViperPluginStatus
	{
		ViperPlugin_Running=0,       /**< Plugin is running */
		/* All states below are unexecutable */
		ViperPlugin_Paused,          /**< Plugin is loaded but paused */
		ViperPlugin_Error,           /**< Plugin is loaded but errored/locked */
		/* All states below do not have all natives */
		ViperPlugin_Loaded,          /**< Plugin has passed loading and can be finalized */
		ViperPlugin_Failed,          /**< Plugin has a fatal failure */
		ViperPlugin_Created,         /**< Plugin is created but not initialized */
		ViperPlugin_Uncompiled,      /**< Plugin is not yet compiled by the JIT */
		ViperPlugin_BadLoad,         /**< Plugin failed to load */
	};


	/**
	 * @brief Describes the object lifetime of a plugin.
	 */
	enum ViperPluginType
	{
		ViperPluginType_Private,    /**< Plugin is privately managed and receives no forwards */
		ViperPluginType_MapUpdated, /**< Plugin will never be unloaded unless for updates on mapchange */
		ViperPluginType_MapOnly,    /**< Plugin will be removed at mapchange */
		ViperPluginType_Global,     /**< Plugin will never be unloaded or updated */
	};

	class IViperPlugin
	{
	public:
		/**
		 * Runs the plug-in
		 *
		 * @note This must be called to execute the plug-in.
		 */
		virtual void RunPlugin() =0;
    
		/**
		 * @brief Retrieves the plug-in's "myinfo" global variable to store inside
		 * this plugin, for use in the main menu.
		 *
		 * @note: This does NOT release/acquire the interpreter lock, and must be
		 *   done manually by the caller.
		 */
		virtual void UpdateInfo() =0;
    
		/**
		 * @brief Returns the path to the script
		 */
		virtual char const *GetPath() =0;
    
		/**
		 * @brief Returns the name of the deepest folder
		 * For example, "test" will be returned if GetPath() returns
		 * "/home/srcds/cstrike/addons/sourcemod/plugins/test/__init__.py"
		 *
		 * @return Deepest folder name
		 */
		virtual char const *GetFolder() =0;
    
		/**
		 * @brief Return the name of the plug-in
		 */
		virtual char const *GetName() =0;
    
		/**
		 * @brief Retrieves the first Python thread state created for this plugin's sub-interpreter
		 */
		virtual PyThreadState *GetThreadState() =0;
    
		/**
		 * @brief Retrieves the Python interpreter state of this plugin's sub-interpreter
		 */
		virtual PyInterpreterState *GetInterpState() =0;
    
		/**
		 * @brief Returns the plugin's status
		 */
		virtual ViperPluginStatus GetStatus() =0;
    
		/**
		 * @brief Returns the type of plug-in, i.e. when it's automatically reloaded
		 */
		virtual ViperPluginType GetType() =0;
    
		/**
		 * @brief Returns the info of the plugin
		 */
		virtual const viper_plugininfo_t *GetPublicInfo() =0;
    
		/**
		 * @brief Sets a property associated with a plugin
		 * @return True on success, false otherwise
		 */
		virtual bool SetProperty(char const *prop, void *value) =0;
    
		/**
		 * @brief Retrieves a property of this plugin
		 * @param ptr   The pointer to write the retrieved value to
		 * @param remove    If true, removes the property
		 */
		virtual bool GetProperty(char const *prop, void **ptr, bool remove=false) =0;
    
		/**
		 * @brief Returns the plug-in's globals
		 */
		virtual PyObject *GetPluginDict() =0;
	};

	/**
	 * Encapsulates a function in a plugin
	 */
	class IViperPluginFunction
	{
	public:
		/**
		 * @brief Executes the function
		 * 
		 * @return The PyObject returned by the function
		 * @note To use the PyObject, the interpreter lock must be released and the
		 *       current thread state must be of the plug-in. See GetOwnerPlugin()
		 */
		virtual PyObject *Execute(PyObject *args, PyObject *keywds=NULL) =0;
    
		/**
		 * @brief Returns the plugin whom owns the function
		 */
		virtual IViperPlugin *GetOwnerPlugin() =0;
    
		/**
		 * @brief Returns the Python function
		 */
		virtual PyObject *GetFunction() =0;
    
		/**
		 * @brief Calls this Python function with the two arguments result and
		 *        fwd_function and expects an integer value for its return. This
		 *        function helps Python functions to be used as callbacks when
		 *        executing a Forward.
		 */
		virtual ViperResultType ForwardCallback(IViperForward *fwd, PyObject *result,
												IViperPluginFunction *fwd_function) =0;
	};

	/**
	 * @brief Listens for plugin-oriented events.
	 */
	class IViperPluginsListener
	{
	public:
		/**
		 * @brief Called when a plugin is created/mapped into memory.
		 */
		virtual void OnPluginCreated(IViperPlugin *plugin)
		{
		}

		/**
		 * @brief Called when a plugin is fully loaded successfully.
		 */
		virtual void OnPluginLoaded(IViperPlugin *plugin)
		{
		}

		/**
		* @brief Called when a plugin is paused or unpaused.
		*/
		virtual void OnPluginPauseChange(IViperPlugin *plugin, bool paused)
		{
		}

		/**
		 * @brief Called when a plugin is unloaded (only if fully loaded).
		 */
		virtual void OnPluginUnloaded(IViperPlugin *plugin)
		{
		}

		/**
		 * @brief Called when a plugin is destroyed.
		 * NOTE: Always called if Created, even if load failed.
		 */
		virtual void OnPluginDestroyed(IViperPlugin *plugin)
		{
		}
	};

	class IViperPluginSys : public ViperInterface
	{
	public: // ViperInterface
		virtual unsigned int GetInterfaceVersion()
		{
			return 1;
		}
    
		virtual char const *GetInterfaceName()
		{
			return "PluginSys";
		}

	public:
		// TODO: Functions to retrieve an IViperPlugin
		/**
		 * @brief Adds a listener to the list
		 * The listener will be notified when plugins load, unload, etc
		 */
		virtual bool AddPluginsListener(IViperPluginsListener *listener) =0;
	};
}

#endif /* _INCLUDE_VIPER_IVIPERPLUGINSYS_H_ */
