About OxygenSDK
===============

This documentation describes plugin development for the
DEWETRON Oxygen measurement software using the OxygenSDK (ODK).
More detailed information about Oxygen can be found at
https://www.dewetron.com/products/software-choices/oxygen/


Structure of the Repository
---------------------------

\/odk/uni

  Contains some low-level utility functions to work with strings,
  xml and UUIDs.

\/odk/base

  Basic data structures that are used for communication between a host
  application and its plugins.

\/odk/api

  The actual interface for Oxygen and Oxygen plugins. Describes messages
  that may be used in both directions and queries to read system information
  from Oxygen. Since some complex commands use XML parameters this
  library also includes (de-)serialization classes for most of them.

  These are still low level commands that should only be used directly if
  some task cannot be solved using the framework functionality.

\/odk/framework

  This library contains support classes that make plugin development
  much easier.

  The base classes, such as :ref:`Software Channel Plugin <software_channel_plugin>`, are tailored for common
  types of plugins and implement basic tasks in order to allow the
  plugin developer to focus on the new functionality.

  Additional helper objects simplify reading input data or exposing
  config items.

\/:ref:`Examples <examples>`

  Source code for several small plugins is provided here to demonstrate
  different scenarios for using the ODK framework.

\/3rdparty

  Contains the source code of third party libraries that are required
  to build the SDK projects.

  pugixml is a fast XML parser for C++ and included on GitHub.

  ODK also requires some parts of the Boost C++ libraries which should
  be extracted to this directory.
  Check out the
  :ref:`Build Instructions <build_instructions>`
  for details.

\/docs

  ODK documentation source files

