============================
Example: Sample Interpolator
============================

This example demonstrates how to read a single scalar SYNC or ASYNC channel and output its values in a possibly interpolated way.

---------
Features
---------
  * Register a new Software Channel Type in Oxygen
  * Plugin instance provides a Channel Id Config Item to configure the input channel and an upsample factor
  * Validate input channel and its type
  * Read samples from synchronous or asynchronous channel
  * Write samples to a synchronous or an asynchronous output channel

::

  Location: examples/sample_interpolator
  Main File: odkex_sample_interpolator.cpp
  Plugin Name: ODK_SAMPLE_INTERPOLATOR
  Plugin UUID: 8A08FFE5-2E71-4A14-8BF5-334873504A8A
