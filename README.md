# DEWETRON OXYGEN SDK
[![Github All Releases](https://img.shields.io/github/downloads/DEWETRON/OXYGEN-SDK/total)]()
[![Build](https://github.com/DEWETRON/OXYGEN-SDK/actions/workflows/ci.yml/badge.svg)](https://github.com/DEWETRON/OXYGEN-SDK/actions/workflows/ci.yml)

## About
Develop your own plugins for DEWETRON Oxygen Software!
The SDK includes the framework, manual and examples to quickly get up and running using the Microsoft Visual Studio C++ toolchain.

The following functionality is currently supported in the SDK and 'Oxygen 6.0' and later:
- Read and Write data from/to numeric channels (Scalar and Vector)
- Create channels
- Create config items for setup save/load and user config
  * Numeric, Text, Channel List
- Get Access to full channel list (read only)
- Get Timebase and start time for sample placing
- Log into Oxygen Logfile
- Custom export plugins
- Offline use

## Coming soon
Several advanced features are not yet available:
- Create an instrument
- Marker management
- Start/Stop measurement or other application control
- Licensing
- ...

If you are interested in one of the non-implemented features, please contact us!

## Documentation
Documentation can be found at https://dewetron.github.io/OXYGEN-SDK

## Requirements
The Oxygen SDK supports development under both Windows and Linux.
It requires the following build environment:
- C++17 compatible compiler
  - Visual Studio 2019 Community Edition or Professional (or newer)
  - GCC/Clang for Linux
- CMake 3.1 or newer
- QT for GUI elements
  - Open-Source version 5.12 or newer is supported
  - A bundle with required components is provided in the [release section](https://github.com/DEWETRON/OXYGEN-SDK/releases)
- Boost 1.70 or newer is used for unit testing (not required to use the SDK)

## Changelog
For a detailed changelog see [CHANGELOG.md](CHANGELOG.md).

# What is DEWETRON Oxygen Software?
The OXYGEN measurement software is the most intuitive and easy-to-use software in the data acquisition sector, designed to be operated via touchscreen or classic mouse and keyboard.
Its nice instruments and controls make it fun to navigate through tons of data.
100% Made in Austria by DEWETRON.

Check out https://www.dewetron.com/products/oxygen-measurement-software/ for more information.


## How to get Oxygen Software?
The current version of Oxygen for SDK users can be downloaded from the customer care center portal at https://ccc.dewetron.com/

- Create a user account
- Navigate to DOWNLOADS -> SOFTWARE & TOOLS -> OXYGEN
(https://ccc.dewetron.com/pl/oxygen)
- Click on the latest DEWETRON Oxygen R7.X.X installer.


# Contact

**Company information**

[www.dewetron.com](https://www.dewetron.com)

**For general questions please contact:**

support@dewetron.com


**For technical questions please contact:**

Gunther Laure

gunther.laure@dewetron.com

Matthias Straka

matthias.straka@dewetron.com


# License
MIT License

Copyright (c) 2024 DEWETRON

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
LICENSE (END)
