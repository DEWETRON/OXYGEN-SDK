==================
Build Instructions
==================

------------
Requirements
------------

Windows:
  * DEWETRON Oxygen 5.0.2 or later (https://ccc.dewetron.com/)
  * Microsoft Visual Studio 2017 with C++ compiler and toolchain (https://visualstudio.microsoft.com/)
  * CMake 3.1 or later (https://cmake.org/download/)
  * Source files for Boost C++ Libraries 1.70 or later
    (https://www.boost.org/users/history/version_1_70_0.html)

Make sure Visual Studio and CMake are installed.
Please verify that the ``cmake`` command can be run from the command prompt.
If this does not work you have use the 'Add CMake to the system PATH' option
during installation or add it to the PATH manually.


--------------------------------------------
Quick Summary for Experienced C++ Developers
--------------------------------------------

  * Boost has to be extracted to to find the header files at
    {repo-root}\\3rdparty\\boost_1_*\\boost

  * Main CMakeLists.txt file in root directory of the repository
    references the SDK and all example plugins.

  * Each example directory contains a CMakeLists.txt that can be
    used directly to build just that example and the necessary dependencies.

  * Plugins are built to {solution-folder}\\{build-type}\\plugins\\*.plugin
    and have to be copied to
    {PublicDocuments}\\Dewetron\\Oxygen\\Plugins or
    {oxygen-install-folder}\\bin to use in Oxygen.

-------------------------
Step by Step Instructions
-------------------------

The following description assumes that the OXYGEN-SDK repository is cloned
to c:\\OXYGEN-SDK.

  * Extract the Boost source archive to c:\\OXYGEN-SDK\\3rdparty.

    Verify that the file
    c:\\OXYGEN-SDK\\3rdparty\\boost_1_70_0\\boost\\align.hpp exists
    to make sure the extracted paths are correct.

  * Open a command line prompt and change to the directory c:\\OXYGEN-SDK

  * Create a new directory 'build' and change to c:\\OXYGEN-SDK\\build

      ``mkdir build``

      ``cd build``

  * Generate a Visual Studio solution by using the following command:

      ``cmake ..``

    If CMake encounters an error you need to fix the issue and then invoke

      ``del CMakeCache.txt``

    before retrying to ensure a clean run.

  * Open the generated Solution in Visual Studio

      ``start ODK.sln``

  * Build one of the example plugins

    The compiled plugins will be stored in
    c:\\OXYGEN-SDK\\build\\{Debug|Release}\\plugins
    as files with the extension .plugin

  * To test a plugin it has to be copied to
    {PublicDocuments}\\Dewetron\\Oxygen\\Plugins
    or the 'bin' directory of the
    installed Oxygen (by default this is
    C:\\Program Files\\DEWETRON\\Oxygen\\bin).

  * Start Oxygen and the plugin will automatically load.

    This can be verified by looking at
    'System Settings' -> 'Extensions and Plugins' -> 'Overview'

