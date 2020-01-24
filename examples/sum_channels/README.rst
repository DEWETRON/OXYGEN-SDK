===========================
Example: Sync+Async Channel
===========================

This example demonstrates how to sum up sample values of two scalar input channels and write the result to an output channel.

---------
Features
---------
  * Register a new Software Channel Type in Oxygen
  * Plugin instance provides a Channel Id List Config Item to configure the two input channels
  * Config Item name is translated to English and German
  * Validate input channels and their type
  * Read samples from synchronous and/or asynchronous channels
  * Write samples to a synchronous and an asynchronous output channel

::

  Location: examples/sync_plus_async_channel
  Main File: odkex_sync_plus_async_channel.cpp
  Plugin Name: ODK_SUM_CHANNELS
  Plugin UUID: D9C295C0-CBB9-4412-9B4A-0C5B1ACF3EB6

