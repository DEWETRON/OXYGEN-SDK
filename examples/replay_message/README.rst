=========================
Example: Async File Replay
=========================

This example channel reads bytes arrays from a CSV file and writes them to a message channel.
The CSV file needs to contain two data columns per message: the timestamp (a number representing seconds) and payload (data bytes of the message writen in hexadecimal format).
Playback runs based on the timestamps and loops once the end of the file is reached.

---------
Features
---------
  * Register a new Software Channel Type in Oxygen
  * Plugin channel provides editable filename (string) config items
  * Custom config item provides an english display name
  * Validate input file and update channel state accordingly
  * Write samples to a asynchronous message channel

::

  Location: examples/replay_message
  Main File: odkex_replay_message.cpp
  Plugin Name: ODK_REPLAY_MESSAGE
  Plugin UUID: A7308ACF-EA6D-4028-AAF3-35F8104837FA

