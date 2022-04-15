==============================
Example: Simple Moving Average
==============================

This example demonstrates how to read a single SYNC input channel and compute an output value (average) from a sliding window
The output of the sliding window is shifted so that there is no phase shift (zero-phase filter).

---------
Features
---------
  * Register a new Software Channel Type in Oxygen for online and offline calculation (with pause support)
  * Plugin instance provides a Channel Id config item and an Unsigned Integer config item to configure the calculation
  * Config Item names are translated to English and German
  * Validate input channel and its type
  * The output channel has a generated default name
  * Read samples from the synchronous input channel into a buffer that is preserved across multiple process() calls
  * Write samples to a synchronous output channel with a different timestamp than the current sample

::

  Location: examples/simple_moving_average
  Main File: odkex_simple_moving_average.cpp
  Plugin Name: ODK_SIMPLE_MOVING_AVERAGE
  Plugin UUID: E9698711-6DC4-4391-8DD0-F3455D64AA98
