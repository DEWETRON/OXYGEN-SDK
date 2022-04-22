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



Development Requirements for Linux
----------------------------------

 * Ubuntu 20.04 LTS Linux (Focal Fossa)
 * Red Hat Enterprise Linux 8 (RHEL8)


The minimum build environment for Linux consists of following packages:

  * cmake
  * gcc/g++
  * boost
  * qt-dev
  * make
  * ninja (optional)

It is completely ok to use the packages provided by the distribution.



Optional Requirements for Linux
-------------------------------

A good and free IDE for Linux (and Windows too) is Visual Studio Code.
The download is free and its usage is highly recommended.

https://code.visualstudio.com/download



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



Build instructions for Linux
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

It is assumed that https://github.com/DEWETRON/OXYGEN-SDK is cloned
to the directory refered to as WORKSPACE:

.. code:: text
   
   PC$ cd $HOME
   PC$ git clone https://github.com/DEWETRON/OXYGEN-SDK.git OXYGEN-SDK


The directory ``/home/USER/OXYGEN-SDK`` has been created containing the latest
SDK version.


Building with the shell
^^^^^^^^^^^^^^^^^^^^^^^

Enter the OXYGEN-SDK directory and execute cmake to check for all necessary
dependencies and let it create a Makefile.

.. code:: text
   
   PC$ cd $HOME/OXYGEN-SDK
   PC$ mkdir build
   PC$ cd build
   PC$ cmake ..
   -- The C compiler identification is GNU 9.4.0
   -- The CXX compiler identification is GNU 9.4.0
   -- Check for working C compiler: /usr/bin/cc
   -- Check for working C compiler: /usr/bin/cc -- works
   -- Detecting C compiler ABI info
   -- Detecting C compiler ABI info - done
   -- Detecting C compile features
   -- Detecting C compile features - done
   -- Check for working CXX compiler: /usr/bin/c++
   -- Check for working CXX compiler: /usr/bin/c++ -- works
   -- Detecting CXX compiler ABI info
   -- Detecting CXX compiler ABI info - done
   -- Detecting CXX compile features
   -- Detecting CXX compile features - done
   -- ODKROOT = /home/USER/OXYGEN-SDK
   -- Performing Test _HAS_CXX17_FLAG
   -- Performing Test _HAS_CXX17_FLAG - Success
   -- Checking C++11 support for "constexpr"
   -- Checking C++11 support for "constexpr": works
   -- GITHUB_REPO = True
   -- Found Boost: /usr/lib/x86_64-linux-gnu/cmake/Boost-1.71.0/BoostConfig.cmake (found version "1.71.0")  
   -- Qt found
   -- Configuring done
   -- Generating done
   -- Build files have been written to: /home/USER/OXYGEN-SDK/build
   

Now run make to build the SDK and all example plugins

.. code:: text
   
   PC$ make
   Scanning dependencies of target pugixml
   [  1%] Building CXX object 3rdparty/pugixml-1.9/CMakeFiles/pugixml.dir/__/src/pugixml.cpp.o
   [  2%] Linking CXX static library ../../Debug/libpugixml.a
   
   lines skipped
   
   [ 98%] Linking CXX shared library ../../Debug/plugins/libex_wav_export.plugin
   [100%] Built target ex_wav_export
  

Building with Visual Studio Code
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

It is highly suggested that following Visual Studio Code extensions have been installed:

 * C/C++
 * CMake Tools
 * alternatively: C/C++ Extension Pack


*Please note that Visual Studio Code is also a good working IDE for Windows.*


Start Visual Studio Code

.. figure:: img/vscode.png
    :alt: Visual Studio Code
    :width: 7in

    Visual Studio Code


Select *Open Folder* and select ``/home/USER/OXYGEN-SDK``. On the first start
Visual Studio Code asks which compiler (or Kit) to use.
Select 9.3 or 9.4, both work great.

.. note:: Most modern compilers will work, as far as C++ 17 is supported.


Usually cmake is automatically run by the IDE.


.. figure:: img/vscode_after_cmake.png
    :alt: Visual Studio Code after cmake
    :width: 7in

    Visual Studio Code after cmake



Press *Build* in the bottom bar to trigger the compilation of the Oxygen-SDK framework
and all example plugins.
There is also the option to switch between Release and Debug build.



.. figure:: img/vscode_build.png
    :alt: Visual Studio Code Building
    :width: 7in

    Visual Studio Code Building


After building all plugins are found here:

*Debug*: OXYGEN-SDK/build/Debug/plugins/

*Release*: OXYGEN-SDK/build/Release/plugins/


.. code:: text
   
   PC$ ls build/Debug/plugins/
   libex_bin_detector.plugin
   libex_property_callback.plugin
   libex_replay_sync_scalar.plugin
   libex_sample_interpolator.plugin
   libex_simple_moving_average.plugin
   libex_sum_channels.plugin
   libex_sync_resample_source.plugin
   libex_wav_export.plugin

