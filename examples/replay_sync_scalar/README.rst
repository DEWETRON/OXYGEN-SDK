=========================
Example: Sync File Replay
=========================

This example channel reads floating point values from the first column of a CSV file and writes them to a synchronous output channel.
Output frequency can be configured using the Sample rate setting. Playback loops when the end of the file is reached.

---------
Features
---------
  * Register a new Software Channel Type in Oxygen
  * Plugin channel provides editable filename (string) and sample rate (scalar) config items
  * Custom config item provides an english display name
  * Validate input file and update channel state accordingly
  * Set maximum range of output channel sample values
  * Write samples to a synchronous scalar output channel
  * UI extension to configure replay file
  * Custom request to demonstrate communication between UI and plugin

::

  Location: examples/replay_sync_scalar
  Main File: odkex_replay_sync_scalar.cpp
  Plugin Name: ODK_REPLAY_SYNC_SCALAR
  Plugin UUID: E70860DB-1A21-4C1E-8329-55A1871ACC7A

