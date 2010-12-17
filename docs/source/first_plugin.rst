======================
My First Viper Plug-in
======================

Viper plug-ins are simple enough to write. They are written in Python (obviously). To learn Python, check out the `official Python docs <http://docs.python.org/>`_ or `dive into python <http://diveintopython.org/>`_.

Folder Layout
=============

Viper plug-ins are placed in the sourcemod/plugins folder, just like SourceMod plugins. Unlike SourceMod plug-ins, however, each Viper plug-in resides in its own folder undearneath sourcemod/plugins/. In the sourcemod/plugins folder, create a new folder named `helloworld`. Inside that folder, create a file named `__init__.py`. This file is the plugin::
  
  addons/
    sourcemod/
      plugins/
        helloworld/
          __init__.py


The Code
========

First off, we need to add some standard information to our plug-in, like the title and author. Inside `__init__.py`, place this code::
    
    import sourcemod
    
    myinfo = {
        'name': "Hello, World!",
        'author': "My Name",
        'description': "Salutations, Celestial Rock!",
        'version': "1.0"
    }

This will inform Viper that our plug-in is named "Hello, World!", it's written by "My Name", and it's version "1.0".

Unlike SourcePawn plug-ins, we don't need to begin our plug-in inside an OnPluginStart function. Code is run from top to bottom, meaning we can place code anywhere. So, to print a message to the server, just do this::
    
    import sourcemod
    
    myinfo = {
        'name': "Hello, World!",
        'author': "My Name",
        'description': "Salutations, Celestial Rock!",
        'version': "1.0"
    }
    
    print "Hello, World!"

And that's all there is to it! When you start up your server, it will load the plug-in and "Hello, World!" will appear in the server console. 
