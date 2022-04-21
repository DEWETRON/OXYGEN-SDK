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

No. Plugins are in fact dynamic link libraries (.dll) on Windows and
shared objects (.so) on Linux.

So one has to provide different Windows and Linux versions of their
plugin. Of course those can be build from the same source code.

