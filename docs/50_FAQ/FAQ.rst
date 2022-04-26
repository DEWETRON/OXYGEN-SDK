
Frequently Asked Questions
==========================



Development Environment
-----------------------


What is cmake?
~~~~~~~~~~~~~~

CMake is an open-source, cross-platform family of tools designed to build,
test and package software.

CMake allows to create Visual Studio solutions, GNU Makefiles, Ninja build
scripts and many more from a single maintainted *CMakeLists.txt* file.



Can a plugin compiled for Windows used with a Linux version of Oxygen?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

No. Plugins are dynamic link libraries (.dll) on Windows and
shared objects (.so) on Linux. They are dependent on the operting
system and the compiler (ABI).

So one has to provide different Windows and Linux versions of their
plugin. Of course those can be build from the same source code.




My plugin builds but Oxygen fails to load it. It does not show up in the plugin list?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Maybe you build the plugin for the wring architecture. Oxygen is a 64bit application (x64)
and is only able load plugins also build for 64bit.

Try to add ``-A x64`` to cmake to get the correct Visual Studio Solution.

.. code:: text
   
   $ cd C:\OXYGEN-SDK
   $ cd build
   $ cmake -A x64 ..
   


Plugin Development
------------------

I am developing using Linux. Is it possible to create a UUID/GUID without Visual Studio?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

On Ubuntu 20.04 you can use the command ``uuidgen``:

.. code:: text
   
   $ uuidgen
   7afa630a-0b15-4f34-b583-14e38c04293d
   


Where should I copy my plugin to so that Oxygen will load it?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Valid locations for plugins are:

Windows:

``C:\Users\Public\Documents\Dewetron\Oxygen\Plugins``

This is a good location for deploying plugins during development.


``C:\Program Files\DEWETRON\OXYGEN\bin``

If you copy a plugin into the Oxygen installation folder it will also work.
Please note that the Oxygen uninstaller removes the complete directory including
all copied plugins.


Linux:

``$HOME/Dewetron/Oxygen/plugins``
