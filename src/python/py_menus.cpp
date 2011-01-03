/**
 * =============================================================================
 * Viper
 * Copyright (C) 2007-2011 Zach "theY4Kman" Kanzler
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
 * ========================
 * = Menu system planning =
 * ========================
 * 
 * Menus in Viper will be constructed using lists of tuples:
 *    my_menu = menus.Menu("Main menu title", callback, [
 *        ("Option 1",),
 *        ("Option 2", callback),
 *        ("Option 3", callback, 'extra argument'),
 *        ("Option 4", "Submenu Title", callback, [
 *            ("Submenu option 1"),
 *            ("Submenu option 2")
 *        ]),
 *        ("Option 5", "Function-defined Submenu", callback, [options_function])
 *        ("Option 6", title_function, callback, [options_function])
 *    ])
 *
 * The constructor of the Menu class will accept a title string and a default
 * callback function that will be used if no callback is set for an individual
 * option. 
 * 
 * The actual menu structure can either be a list of tuples or a list containing
 * a function that returns a list of tuples. The first element of every tuple
 * will be the text displayed for that option in the menu. The rest of the
 * elements depend on whether the option is a submenu or not.
 * 
 * If the option is not a submenu, the second element is optionally a callback
 * function that will be called when the option is selected. This overrides the
 * callback passed to the menus.Menu constructor. The third element is an
 * optional extra argument to pass to the callback function.
 * 
 * If the option is a submenu, the second element of the tuple must be either a
 * string containing the submenu title, or a function that returns a string
 * containing the submenu title. The third element must be a list. The inside
 * of this list carries the same structure as defined above.
 */

#include <Python.h>
#include <sh_list.h>
#include "IViperPluginSys.h"
#include "PluginSys.h"
#include "viper_globals.h"

using SourceHook::List;

class ViperMenu;

struct menus__Menu
{
    PyObject_HEAD
    
    ViperMenu *menu;
};

struct ViperMenuStruct
{
    ViperMenuStruct() : title(NULL), callback(NULL), extra(Py_None), menu(NULL)
    {
    }
    
    char const *title;
    IViperPluginFunction *callback;
    PyObject *extra;
    ViperMenu *menu;
};

class ViperMenu
{
public:
    ViperMenu(char const *title, IViperPluginFunction *callback)
        : m_Callback (callback), m_TitleFunc(NULL)
    {
        m_Title = sm_strdup(title);
        m_Options = new List<ViperMenuStruct>();
    }
    
    ViperMenu(IViperPluginFunction *title_func, IViperPluginFunction *callback)
        : m_Callback (callback), m_TitleFunc(title_func), m_Title(NULL)
    {
        m_Options = new List<ViperMenuStruct>();
    }
    
    ViperMenu(char const *title, IViperPluginFunction *callback,
        IViperPluginFunction *options)
        : m_Callback (callback), m_Options(NULL), m_OptionsFunction(options)
    {
        m_Title = sm_strdup(title);
    }
    
    ViperMenu(IViperPluginFunction *title_func, IViperPluginFunction *callback,
        IViperPluginFunction *options)
        : m_Callback (callback), m_TitleFunc(title_func), m_Title(NULL),
          m_OptionsFunction(options), m_Options(NULL)
    {
    }
    
    ~ViperMenu()
    {
        if (m_TitleFunc == NULL && m_Title != NULL)
            delete [] m_Title;
    }
    
    void DisplayMenu()
    {
    }
    
    void AddMenuStruct(const ViperMenuStruct &menu)
    {
        m_Options->push_back(menu);
    }
    
    void AddMenuOption(char const *text)
    {
        ViperMenuStruct opt;
        opt.title = sm_strdup(text);
        opt.callback = m_Callback;
        
        AddMenuStruct(opt);
    }
    
    void AddMenuOption(char const *text, IViperPluginFunction *callback)
    {
        ViperMenuStruct opt;
        opt.title = sm_strdup(text);
        opt.callback = callback;
        
        AddMenuStruct(opt);
    }
    
    void AddMenuOption(char const *text, IViperPluginFunction *callback,
        PyObject *extra)
    {
        ViperMenuStruct opt;
        opt.title = sm_strdup(text);
        opt.callback = callback;
        opt.extra = extra;
        
        AddMenuStruct(opt);
    }
    
    void AddMenuOption(ViperMenu *menu)
    {
        ViperMenuStruct opt;
        opt.menu = menu;
        
        AddMenuStruct(opt);
    }
    
    void SetOptionsFunction(IViperPluginFunction *options_func)
    {
        m_OptionsFunction = options_func;
    }

private:
    List<ViperMenuStruct> *m_Options;
    char const *m_Title;
    IViperPluginFunction *m_TitleFunc;
    IViperPluginFunction *m_Callback;
    IViperPluginFunction *m_OptionsFunction;
};

ViperMenu *
ParseMenuTuple(PyObject *args)
{
    // Parse arguments for (title, callback, list)
    PyFunction *py_title;
    PyFunction *py_callback;
    PyObject *py_list;
    
    if (!PyArg_ParseTuple(args, "OOO!",  &py_title, &py_callback, _PyList_Type,
        &py_list))
    {
        return NULL;
    }
    
    // Check callback is callable
    if (!PyCallable_Check(py_callback))
    {
        PyErr_Format(_PyExc_TypeError, "callback should be callable (type %s)",
            py_callback->ob_type->tp_name);
        return NULL;
    }
    
    // Get thread plugin
    GET_THREAD_PLUGIN();
    IViperPluginFunction *func = CPluginFunction::CreatePluginFunction(
        py_callback, pPlugin);
    
    List<ViperMenu *> *menus;
    if (!pPlugin->GetProperty("Menus", (void **)&menus))
    {
        menus = new List<ViperMenu *>();
        pPlugin->SetProperty("Menus", (void *)menus);
    }
    
    ViperMenu *menu;
    
    // Check py_title is either title function or title string
    if (PyCallable_Check(py_title))
    {
        IViperPluginFunction *title_func = 
            CPluginFunction::CreatePluginFunction(py_title, pPlugin);
        menu = new ViperMenu(title_func, func);
    }
    else if (PyString_Check(py_title))
        menu = new ViperMenu(PyString_AS_STRING(py_title), func);
    else
    {
        PyErr_Format(_PyExc_TypeError, "title is wrong type (should be str or"
            " callable, got \"%s\")", py_title->ob_type->tp_name);
        return NULL;
    }
    
    /* If the list has one element and it's callable, it's an options func */
    Py_ssize_t len = PyList_Size(py_list);
    if (len == 1 && PyCallable_Check(PyList_GetItem(py_list, 0)))
    {
        PyFunction *py_options_func = PyList_GetItem(py_list, 0);
        IViperPluginFunction *options_func =
            CPluginFunction::CreatePluginFunction(py_options_func, pPlugin);
        menu->SetOptionsFunction(options_func);
    }
    
    // Parse list (see explanation above)
    for (Py_ssize_t i=0; i<len; i++)
    {
        PyObject *option = PyList_GetItem(py_list, i);
        if (!PyTuple_Check(option))
        {
            PyErr_Format(_PyExc_TypeError, "Menu options should be tuples"
                " (expected tuple, found \"%s\")", option->ob_type->tp_name);
            delete menu;
            return NULL;
        }
        
        PyObject *py_title;
        PyFunction *callback = NULL;
        IViperPluginFunction *func = NULL;
        Py_ssize_t optlen = PyTuple_Size(option);
        
        if (optlen == 0)
        {
            PyErr_Format(_PyExc_TypeError, "Menu option tuples must have at"
                " least one element; there are %u.", optlen);
            delete menu;
            return NULL;
        }
        else if (optlen > 4)
        {
            PyErr_Format(_PyExc_TypeError, "Menu option tuples must have at"
                " most four elements; there are %u.", optlen);
            delete menu;
            return NULL;
        }
        
        // Pull out the title
        py_title = PyTuple_GetItem(option, 0);
        if (!PyString_Check(py_title))
        {
            PyErr_Format(_PyExc_TypeError, "Menu option element 1 should be"
                " string (expected str, found \"%s\")",
                py_title->ob_type->tp_name);
            delete menu;
            return NULL;
        }
        char const *title = PyString_AS_STRING(py_title);
        
        if (optlen == 1)
        {
            menu->AddMenuOption(title);
            continue;
        }
        
        // If the option has 4 elements, it's a submenu
        if (optlen == 4)
        {
            PyObject *py_submenu = PyTuple_GetSlice(option, 1, optlen);
            ViperMenu *submenu = ParseMenuTuple(py_submenu);
            
            if (submenu == NULL)
            {
                delete menu;
                Py_XDECREF(py_submenu);
                
                return NULL;
            }
            
            menu->AddMenuOption(submenu);
        }
        else
        {
            PyObject *py_callback = PyTuple_GetItem(option, 1);
            if (!PyCallable_Check(py_callback))
            {
                PyErr_Format(_PyExc_TypeError, "Non-submenu options must have"
                    " a callable as their second element (expected callable,"
                    " got \"%s\")", py_callback->ob_type->tp_name);
                delete menu;
                return NULL;
            }
            
            IViperPluginFunction *callback =
                CPluginFunction::CreatePluginFunction(py_callback, pPlugin);
            
            if (optlen == 2)
            {
                menu->AddMenuOption(title, callback);
                continue;
            }
            
            PyObject *py_extra = PyTuple_GetItem(option, 2);
            menu->AddMenuOption(title, callback, py_extra);
            continue;
        }
    }
    
    return menu;
}

static int
menus__Menu__init__(menus__Menu *self, PyObject *args, PyObject *keywds)
{
    ViperMenu *menu = ParseMenuTuple(args);
    if (menu == NULL)
        return -1;
    
    self->menu = menu;
    return 0;
}

static PyObject *
menus__Menu__repr__(menus__Menu *self)
{
    /* TODO */
    return PyString_FromString("");
}

PyTypeObject menus__MenuType = {
    PyObject_HEAD_INIT(_PyType_Type)
    0,                                                      /*ob_size*/
    "sourcemod.menus.Menu",                                 /*tp_name*/
    sizeof(menus__Menu),                                    /*tp_basicsize*/
    0,                                                      /*tp_itemsize*/
    0,                                                      /*tp_dealloc*/
    0,                                                      /*tp_print*/
    0,                                                      /*tp_getattr*/
    0,                                                      /*tp_setattr*/
    0,                                                      /*tp_compare*/
    (reprfunc)menus__Menu__repr__,                          /*tp_repr*/
    0,                                                      /*tp_as_number*/
    0,                                                      /*tp_as_sequence*/
    0,                                                      /*tp_as_mapping*/
    0,                                                      /*tp_hash */
    0,                                                      /*tp_call*/
    0,                                                      /*tp_str*/
    0,                                                      /*tp_getattro*/
    0,                                                      /*tp_setattro*/
    0,                                                      /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE,                 /*tp_flags*/
    "A Valve menu structure",                               /* tp_doc */
    0,                                                      /* tp_traverse */
    0,                                                      /* tp_clear */
    0,                                                      /* tp_richcompare */
    0,                                                      /* tp_weaklistoffset */
    0,                                                      /* tp_iter */
    0,                                                      /* tp_iternext */
    0,                                                      /* tp_methods */
    0,                                                      /* tp_members */
    0,                                                      /* tp_getset */
    0,                                                      /* tp_base */
    0,                                                      /* tp_dict */
    0,                                                      /* tp_descr_get */
    0,                                                      /* tp_descr_set */
    0,                                                      /* tp_dictoffset */
    (initproc)menus__Menu__init__,                          /* tp_init */
    0,                                                      /* tp_alloc */
    &PyType_GenericNew,                                     /* tp_new */
};

PyObject *
initmenus(void)
{
    if (PyType_Ready(&menus__MenuType) < 0)
    {
        PyErr_Print();
        return NULL;
    }
    
    PyObject *menus = Py_InitModule3("menus", NULL, "Handling of Valve menus.");
    
    Py_INCREF((PyObject *)&menus__MenuType);
    PyModule_AddObject(menus, "Menu", (PyObject *)&menus__MenuType);
    
    return menus;
}
