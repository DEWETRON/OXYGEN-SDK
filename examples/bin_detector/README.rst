=====================
Example: Bin Detector
=====================

This example plugin detects the minimum and/or maximum values with the
corresponding bins out of an Oxygen vector channel (e.g. a fft channel).
All values and bins are written to separate asynchronous output channels.
Detecting min/max values and bins can be configured through the plugin
group channel setup page.

---------
Features
---------
  * Register a new Software Channel Type in Oxygen
  * Plugin group channel provides selectable config items for detecting min
    and/or max values and their corresponding bins.
  * Creates output channels for values and bins dynamically. The name of
    the selected input channel affects the name of the created output
    channel names
  * Read samples from a vector channel
  * Write values and bins to corresponding asynchronous scalar output channels
  * Dynamic plugin config is stored and can be reloaded through oxygen
    save/load config use case

::

  Location: examples/bin_detector
  Main File: odkex_bin_detector.cpp
  Plugin Name: ODK_BIN_DETECTOR
  Plugin UUID: DCDF634A-377B-4E9F-89BD-09D54C9DFCD3

