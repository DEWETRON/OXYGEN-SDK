// Copyright DEWETRON GmbH 2019

#ifndef _ODKAPI_CONFIG_ITEM_KEYS_H_
#define _ODKAPI_CONFIG_ITEM_KEYS_H_

/**
 * Below you can find commonly used config item keys and the description of their associated values
 * Config keys with a colon-separated prefix are reserved for special purposes (e.g. "ID:...")
 */

/**
 * Channel Configuration
 * ---------------------
 * Not every channel provides every item listed below.
 * Acquisition hardware plugins are free to add custom properties as they see fit.
 * Unknown config items should not be changed without good reason.
 */

/// User-specified name of the channel. Not necessarily unique.
/// Value: String
#define ODK_CI_KEY_CHANNEL_NAME "Neon/Name"

/// Additional context identifier for the channel, which can be used to disambiguate when displaying the channel name
/// Value: String
#define ODK_CI_KEY_CHANNEL_DOMAIN "Neon/DomainUrl"

/// The long name of a channel consists of the name and the domain of the channel if this is necessary for disambiguation. Also not necessarily unique, because channels names in the same domain can be equal.
/// Value: String
#define ODK_CI_KEY_CHANNEL_LONG_NAME "Neon/LongName"

/// Indicates whether the channel should acquires data during acquisition
/// Value: Bool
#define ODK_CI_KEY_CHANNEL_ACTIVE "Neon/Active"

/// By default, all active channels are also stored when recording. If this item is set to NO the channel data is processed during acquisition but never written into a file.
/// Value: Enum (AUTO, NO)
#define ODK_CI_KEY_CHANNEL_STORED "Neon/Stored"

/// Indicates the measurement mode of the channel
/// Value: String
#define ODK_CI_KEY_CHANNEL_MODE "Mode"

/// The unit of the samples that are stored in this channels (after scaling)
/// Value: String
#define ODK_CI_KEY_CHANNEL_PHYSICAL_UNIT "Neon/PhysicalUnit"

/// The type of channel (Analog, Counter...)
/// Value: Enum (DAQChannelType)
#define ODK_CI_KEY_CHANNEL_TYPE "ChannelType"

/// Sample rate (=expected frequency of samples) of the channel
/// For synchronous channels this always implies the temporal distance between two successive samples.
/// For asynchronous channel it contains the expected (~average) sample rate.
/// Value: Scalar
#define ODK_CI_KEY_SAMPLE_RATE "SampleRate"

/// Sample dimension (=number of samples per scan) of the channel
/// Value: Uint
#define ODK_CI_KEY_SAMPLE_DIMENSION "SampleDimension"

/// Used - flag of the channel
/// Value: bool
#define ODK_CI_KEY_USED "Used"

#endif //_ODKAPI_CONFIG_ITEM_KEYS_H_
