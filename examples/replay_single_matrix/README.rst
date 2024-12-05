=========================
Example: Single Matrix File Replay
=========================

This example channel reads floating point values from a CSV file and writes them to a single-value matrix output channel at sample 0.

A matrix CSV file can look like this:
.. code-block:: plaintext
    :caption: Example input file for a matrix

    1;2;-3;4
    5;2;-3;0
    0;3;-1;2

A vector CSV file can look like this:
.. code-block:: plaintext
    :caption: Example input file for a vector

    1;2;-3;4;5;2;-3;0

::

  Location: examples/replay_single_matrix
  Main File: odkex_replay_single_matrix.cpp
  Plugin Name: ODK_REPLAY_SINGLE_MATRIX
  Plugin UUID: 102B673A-0F6B-457D-B1D4-19174865215B
