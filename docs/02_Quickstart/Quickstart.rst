.. _quickstart:

Quickstart
==========

Lets dive in fast! This quickstart guide will guide you through the
steps necessary for building an Oxygen plugin.


Before using the SDK, please install the supported development tools:

* Visual Studio 2019 or better (for Windows)
* cmake
* Oxygen




Install Oxygen
--------------

Please download and install the latest Oxygen installer from DEWETRON's
download portal:

https://www.dewetron.com/products/oxygen-measurement-software/



Development Requirements for Windows
------------------------------------

  * Microsoft Visual Studio 2019 or later with C++ compiler and toolchain (https://visualstudio.microsoft.com/)
  * CMake (https://cmake.org/download/)
  * Source files for Boost C++ Libraries 1.70 or later
    (https://www.boost.org/users/history/version_1_70_0.html)
  * Git for windows or https://desktop.github.com/ application

Make sure Visual Studio and CMake are installed.
Please verify that the ``cmake`` command can be run from the command prompt.
If this does not work you have use the 'Add CMake to the system PATH' option
during installation or add it to the PATH manually.

.. code:: text

   $ cmake --version
   cmake version 3.16.4
   
   CMake suite maintained and supported by Kitware (kitware.com/cmake).


Optional Requirements
---------------------

For resource package creation the qt resource compiler (rcc) is needed.

It is found the GitHub projects download section:

https://github.com/DEWETRON/OXYGEN-SDK/releases/download/OXYGEN-SDK-6.0.0/qt_resource_compiler.zip


Alternatively one can install the complete Qt libraries package:

  * Qt 5.15.2 (https://www.qt.io/download-qt-installer)

Qt is a rather large download (40GB with included debug symbol files) so
the use of the small provided qt_resource_compiler.zip archive is preferred.



.. _build_instructions:

Build instructions
------------------


Build instructions for Windows
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

It is assumed that https://github.com/DEWETRON/OXYGEN-SDK is cloned
to the directory refered to as WORKSPACE.

Example creating a Oxygen SDK workspace in C:\\OxygenSDK:

.. code:: text
   
   $ cd C:\
   $ git clone https://github.com/DEWETRON/OXYGEN-SDK.git OXYGEN-SDK


{WORKSPACE} refers to the directory C:\\OXYGEN-SDK.


The following description assumes that the OXYGEN-SDK repository is cloned
to ``c:\\OXYGEN-SDK``.

  * Extract the Boost source archive to ``c:\\OXYGEN-SDK\\3rdparty``.

    Verify that the file
    ``c:\\OXYGEN-SDK\\3rdparty\\boost_1_70_0\\boost\\align.hpp`` exists
    to make sure the extracted paths are correct.

  * Unpack ``qt_resource_compiler.zip`` to ``c:\\OXYGEN-SDK``

  * *Alternatively: Qt 5.15.2 has to be installed on the system. This manual assumes the correct build
    for your compiler is installed to {QT_DIR} (for example c:\\Qt\\5.15.2\\msvc2017_64)*

  * Open a command line prompt and change to the directory ``c:\\OXYGEN-SDK``

  * Create a new directory ``build`` and change to ``c:\\OXYGEN-SDK\\build``

      ``mkdir build``

      ``cd build``

  * Generate a Visual Studio solution by using the following command:

      ``cmake -A x64 ..``

    If your are using a qt installation instead of the qt_resource_compiler package please call cmake like this:
    
      ``cmake -A x64 -DCMAKE_PREFIX_PATH={QT_DIR} ..``

    If CMake encounters an error you need to fix the issue and then invoke

      ``del CMakeCache.txt``

    before retrying to ensure a clean run.

  * Open the generated Solution in Visual Studio

      ``start ODK.sln``

  * Build one of the example plugins

    The compiled plugins will be stored in
    ``c:\\OXYGEN-SDK\\build\\{Debug|Release}\\plugins``
    as files with the extension .plugin

  * To test a plugin it has to be copied to
    ``{PublicDocuments}\\Dewetron\\Oxygen\\Plugins``
    or the 'bin' directory of the
    installed Oxygen (by default this is
    ``C:\\Program Files\\DEWETRON\\Oxygen\\bin)``.

  * Start Oxygen and the plugin will automatically load.

    This can be verified by looking at
    'System Settings' -> 'Extensions and Plugins' -> 'Overview'


.. attention:: Oxygen is a 64bit application (x64) and is not able to load plugins build for a 32bit (x86) architecture.
  Please check the build configuration in Visual Studio if your plugin fails to load. Add ``-A x64`` to the cmake call.



Build instructions for Ubuntu
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

TBD


Build instructions for Red Hat Enterprise Linux
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

TBD


Debugging from Visual Studio
----------------------------

The following step-by-step guide explains how to attach a Visual Studio
debugger to a running Oxygen instance in order to debug an ODK plugin
on a developer machine.

  * Make sure the latest build of your plugin is copied to a suitable
    plugin location (see previous section). Usually, copying the
    ``.plugin`` file is sufficient and the ``.pdb`` does not need to be copied.

  * Start Oxygen and make sure your plugin is loaded

  * In Visual Studio, open the debug attach dialog from 'Debug' -> 'Attach to process...'

  * Make sure that the value of 'Attach to:' is set to 'Native code' and not 'Automatic'

  * Select 'Oxygen.exe' in the process list

  * Close the dialog by clicking on the 'Attach' button

  * Now, set your breakpoints and use the plugin inside Oxygen
    until it triggers a breakpoint
