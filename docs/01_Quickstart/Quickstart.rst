.. _build_instructions:

Build Instructions
==================


Requirements
------------

Windows:
  * DEWETRON Oxygen 5.3.0 or later (https://ccc.dewetron.com/)
  * Microsoft Visual Studio 2017 or 2019 with C++ compiler and toolchain (https://visualstudio.microsoft.com/)
  * CMake 3.1 or later (https://cmake.org/download/)
  * Source files for Boost C++ Libraries 1.70 or later
    (https://www.boost.org/users/history/version_1_70_0.html)

Make sure Visual Studio and CMake are installed.
Please verify that the ``cmake`` command can be run from the command prompt.
If this does not work you have use the 'Add CMake to the system PATH' option
during installation or add it to the PATH manually.


Optional Requirements
---------------------

Windows:
  * Qt 5.12.x (https://www.qt.io/download-qt-installer)

As Qt is a rather large downwload (40GB with included debug symbol files)
we provide a minimal archive (qt_resource_compiler.zip) in the Releases section.




Quick Summary for Experienced C++ Developers
--------------------------------------------

  * Boost has to be extracted to find the header files at
    {repo-root}\\3rdparty\\boost_1_*\\boost

  * Optionally: Qt 5.12.x has to be installed on the system.
    This manual assumes the correct build for your compiler is installed to {QT_DIR} (for example c:\\Qt\\5.12.6\\msvc2017_64)

  * Alternative: extract `qt_resource_compiler.zip` to the SDK workspace (this file can be found in the github releases (https://github.com/DEWETRON/OXYGEN-SDK/releases))

  * Main CMakeLists.txt file in root directory of the repository
    references the SDK and all example plugins.
    Run cmbuild with the option -DCMAKE_PREFIX_PATH={QT_DIR}.

  * Each example directory contains a CMakeLists.txt that can be
    used directly to build just that example and the necessary dependencies.

  * Plugins are built to {solution-folder}\\{build-type}\\plugins\\*.plugin
    and have to be copied to
    {PublicDocuments}\\Dewetron\\Oxygen\\Plugins or
    {oxygen-install-folder}\\bin to use in Oxygen.


Step by Step Instructions
-------------------------

The following description assumes that the OXYGEN-SDK repository is cloned
to ``c:\\OXYGEN-SDK``.

  * Extract the Boost source archive to ``c:\\OXYGEN-SDK\\3rdparty``.

    Verify that the file
    ``c:\\OXYGEN-SDK\\3rdparty\\boost_1_70_0\\boost\\align.hpp`` exists
    to make sure the extracted paths are correct.

  * Either install Qt to ``c:\\Qt\\5.12.6``
    or unpack ``qt_resource_compiler.zip`` to ``c:\\OXYGEN-SDK``

  * Open a command line prompt and change to the directory ``c:\\OXYGEN-SDK``

  * Create a new directory ``build`` and change to ``c:\\OXYGEN-SDK\\build``

      ``mkdir build``

      ``cd build``

  * Generate a Visual Studio solution by using the following command:

      ``cmake -DCMAKE_PREFIX_PATH=c:\\Qt\\5.12.6\\msvc2017_64 ..``

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
