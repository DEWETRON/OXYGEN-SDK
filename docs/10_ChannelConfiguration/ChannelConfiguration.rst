.. _channel_configuration:

Oxygen Channel Configuration
============================

The properties of Oxygen channels are managed in the Configuration Chain,
a hierarchy of layers, each containing an arbitrary number of Config Items.
Every module that contributes to the makeup of a channel also adds its own
layer. The source of a channel (hardware or software channel plugin)
provides the first layer, which should contain all configuration and
identification parameters that are relevant on that level.

Intermediate elements can extend the channel by providing new parameters
that influence for example the scaling of physical values or automatically
setup chain elements below them. The final element in the chain is
contributed by Oxygen and stores very generic information like activation
state, name and color or the user scaling parameters.

Conceptually the sample data travels from the first to the last element,
and processing or scaling steps are applied to the value accordingly until
the physical value can be shown to the user. Configuration works the other
way around: Config Items of later chain elements can replace or hide those
below. If multiple items using the same key are present in the chain, only
to last one and its constraints are considered for display and manipulation
by the user.

As an example, consider an element that performs unit conversion for the
sample data: This element would add a string config item where the user can
enter the desired unit. If that unit differs from the unit below (e.g. from
the source) the unit converter adds a new range item, that converts the
range item below into the new unit and indicates actual scaled range.
Addionally it adds a scaling step to the configuration chain to instruct
Oxygen on how the unit conversion has to happen for individual samples.
During acquisition the raw sample value is read from the source, all
scaling steps across the configuration chain are applied in order to the
value, and this final value is shown in instruments.

.. _config_items:

Config Items
------------
A config item is a single named property that has a current value of a
specific type.
Optionally it can contain constraints which describe how this property
should be used in Oxygen. For example constraints indicate whether an item
is editable in UI and which values are allowed.

Based on the configuration and intrinsic knowledge of its owner, each
active channel needs to specify its timebase and data format before it
can be used in Oxygen.

.. _config_item_key:

Name / Key
----------

Many Config Items are predefined by Oxygen and imply a certain semantic.
Those may be used in plugin channels if the channels are compatible with
this behaviour.

Plugins are allowed to add arbitrary items to their channels to store
configuration and identification data. The name of these custom items
has to conform to the following patterns:

- "UNIQUE_PLUGIN_NAME/MyCustomName"
- "ID:UNIQUE_PLUGIN_NAME/MyCustomIdentifier"

All other config items keys are reserved for Oxygen and other plugins
and may not be used for custom items.
It is recommended to use CamelCase ASCII characters for the identifier
and avoid white space.

ID item are considered internal, are therefore not shown to the user,
and may be used to store identification data that is not part of the
mutable configuration. For example, if a plugin supports hardware
channels, this might contain some kind of identifier that allows
the plugin to match the actual configuration to the correct hardware
channel when a setup is loaded.

Values
------

Config items can accept many different types of values.
For plugin development the most important ones are

- String: A Unicode string of aribtrary length, encoded in UTF-8.
- Scalar: A double precision floating point value with an
  associated unit (string usually containing SI units).
- ChannelId(List): one or more unsigned 64bit values that uniquely
  identify a channel in the current Oxygen measurement setup.
- FloatingPointNumber, Integer, UnsignedInteger(64):
  Simple numerical values corresponding to the C types
  double, int32_t, uint32_t, uint64_t respectively.

Constraints
-----------

As mentioned above, Constraints are hints from owner of a config item
on how Oxygen should the treat the item. For example they may indicate
whether to automatically show the item to the user or which values can
be entered by the user. Since these hint can not always precisely
describe the allowed values, it can be necessary to do final validation
and correction of new values when they are sent to the plugin.

An item is only editable if any constraints indicate that another value
is allowed, e.g. two options or a regular expression. Multiple of these
constraints can be present and a value that is allowed by any of them
is considered acceptable.

It is possible to mix constraints allowing different types of values if
handled properly in the plugin. For example it is possible allow a
numeric range as well as the string option "AUTO".

Visibility
''''''''''
Indicates if the item should be visible to the user as a column in the
channel list or in the details view.
Currently supported values are "PUBLIC" (the default) which makes it visible
where appropriate, and "HIDDEN", which enures the user never sees it.

Option
''''''
The exact value provided in the option constraint is considered valid.
Therefore this always sets the identical value & type if picked by the user.

Range
'''''
Implies that any floating point value between Min and Max (inclusive)
may be entered.
This produces a value of type Floating point or Scalar.

ArbitraryString
'''''''''''''''
If present this indicates that an arbitrary string may be entered by the user.
This always produces a value of type String.

RegEx
'''''
If present this indicates that an any string that matches the
regular expression can be entered.
This always produces a value of type String.

ChannelIds
''''''''''
Implies that one or more channel ids should be stored in the config item.
At the moment this cannot be mixed with any other value constraints.

.. _channel_data_format:


Data Format
-----------

The content of a data channel in Oxygen is described by a tuple
containing information about how a single sample is structured
and how often a new value occurs relative to the Time Base frequency.

Occurrence
----------

- Synchronous: the channel stores exactly one sample for every
  new tick of the Time Base.

- Asynchronous: new samples occur with varying gaps between them.
  That means each sample carries a timestamp
  (measured in ticks of the associated Time Base).
  Event though that often indicates of problem of the
  Time Base frequency, it is also possible to store
  multiple samples using the same timestamp.

- Single Value: Conceptually these channels store only a single
  value.

  For example this could be an aggregate statistics
  value that covers the entire duration of a
  measurement.

  But to provide continuous updates and reliable storage
  values should actually be written at sensible
  intervals as if it were an asynchronous channel.
  Oxygen takes care of persisting only the last
  sample at measurement end.

- Never: This indicates that the channel will not contain data.
  Usually this is used for group channels that primarily
  serve as containers to logically group channels and
  configuration.

Regardless of which type of occurrence is active, new samples always
have to be sent to oxygen with monotonically increasing timestamps.

Sample Format & Dimension
-------------------------

The structure of an individual sample is primarily determined by its
format and dimension.

Format is the data type of an indivdual element of the sample;
Dimension is the number of elements of this type that make of the
sample.

For example a channel read from a TRION board might have
the sample format 'sint24' and dimension 1, because the hardware
provides a single signed 24bit value per measurement.

An FFT calculation on the other might output to a vector channel
with a dimension of several thousand elements and store complex
floating point numbers.

A Dimension of 0 indicates that each sample has a different number
of elements.

Not all combinations of Occurrence, Format & Dimension are currently
supported by Oxygen.


Time Base
---------

The time base of a channel describes how its samples are placed on
the measurement timeline.

Each sample contains an implicit (for synchronous channels it starts
at 0 for the first sample and increases by for for each successive
one) or explict (for asynchronous channels) timestamp that is
measured in relative the timebase.

For plugins Oxygen currently supports only the Simple Time Base.
Its only parameter is the frequency (a double precision floating
point value measured in Hertz) of the ticks. Simple time bases
begin running at acquisition start; therefore tick 0 always
indicates the first sample at the time of acquisition start,
regardless of the frequency.

If a synchronous channel uses a Time Base frequency of 2 Hz the
first three samples at tick 0, 1 & 2 are shown at 0.0s, 0.5s
and 1.0s in all instruments.

Other channels or modules may rely on the timebase information 
of the plugins output channels. For example a user might want
to perform an FFT calculation on the output channel.
As a consequence, the timebase of synchronous output channels should
be set as early as possible. In a typical SoftwareChannel implementation 
this means updating the timebase during an update() call.
For synchronous channels the call to setSimpleTimebase of an output channel
automatically updates the "SampleRate" property of the given channel.
