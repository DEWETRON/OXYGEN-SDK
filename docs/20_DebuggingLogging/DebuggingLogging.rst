
Debugging and Logging
=====================

During development both *Debugging* and  *Logging* are essential.
This chapter describes how both con be used when developing Oxygen plugins.


Debugging from Visual Studio
----------------------------




Let the debugger start Oxygen
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

One way to debug the plugin from the initiation phase on is by starting
Oxygen by Visual Studio.

For this the configuration has to be changed a little:

Change the *Command* from *$(TargetPath)* to the Oxygen.exe path:

.. figure:: img/debugging_oxygen_1.png
    :alt: Debugger settings
    :width: 5in

    Debugger Settings.


Set a breakpoint to test if debugging works. We set it in the method
*getSoftwareChannelinfo()* which is always called on plugin instantiation.


.. figure:: img/debugging_oxygen_2.png
    :alt: Setting a breakpoint
    :width: 4in

    Setting a breakpoint.


Now start the debugger by pressing the key *F5* or by selecting *Start Debugging*
in the *Debug* menu.

.. figure:: img/debugging_oxygen_3.png
    :alt: Starting the debugger
    :width: 3in

    Starting the debugger.


Oxygen will start and after a few seconds the execution will stop at the breakpoint.

.. figure:: img/debugging_oxygen_4.png
    :alt: breakpoint hit
    :width: 7in

    Debugging breakpoint hit.


Voila!


Attach the debugger to Oxygen 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Alternatively it is possible to attach a debugger to a running Oxygen 
process.

The following step-by-step guide explains how to attach a Visual Studio
debugger to a running Oxygen instance in order to debug an ODK plugin
on a developer machine.

Make sure the latest build of your plugin is copied to a suitable
plugin location (see previous section). Usually, copying the
``.plugin`` file is sufficient and the ``.pdb`` does not need to be copied.


Start Oxygen and make sure your plugin is loaded.


In Visual Studio, open the debug attach dialog from 'Debug' -> 'Attach to Process...'

.. figure:: img/debugging_oxygen_attach_1.png
    :alt: attach to process
    :width: 3in

    Attach to Process.

Make sure that the value of 'Attach to:' is set to 'Native code' and not 'Automatic'

Select 'Oxygen.exe' in the process list

.. figure:: img/debugging_oxygen_attach_2.png
    :alt: attach to Oxygen
    :width: 5in

    Attaching to Oxygen.

Close the dialog by clicking on the 'Attach' button

Now, set your breakpoints and use the plugin inside Oxygen until it triggers a
breakpoint.



Logging
-------

Logging is still work in progress.
