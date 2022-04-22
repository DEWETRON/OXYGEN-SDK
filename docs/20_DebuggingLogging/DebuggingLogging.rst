
Debugging and Logging
=====================



Debugging from Visual Studio
----------------------------

The following step-by-step guide explains how to attach a Visual Studio
debugger to a running Oxygen instance in order to debug an ODK plugin
on a developer machine.

  * Make sure the latest build of your plugin is copied to a suitable
    plugin location (see previous section). Usually, copying the
    ``.plugin`` file is sufficient and the ``.pdb`` does not need to be copied.

  * Start Oxygen and make sure your plugin is loaded

  * In Visual Studio, open the debug attach dialog from 'Debug' -> 'Attach to process...'

  * Make sure that the value of 'Attach to:' is set to 'Native code' and not 'Automatic'

  * Select 'Oxygen.exe' in the process list

  * Close the dialog by clicking on the 'Attach' button

  * Now, set your breakpoints and use the plugin inside Oxygen
    until it triggers a breakpoint
