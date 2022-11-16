# Change Log

All notable changes to this project will be documented in this file

## [6.3.1] - 2022-11-16
### Changed
- Fixed odk::trim (and variants) to accept UTF-8 strings

## [6.3.0] - 2022-11-02
### Added
- Support for 64-bit Signed Integer properties (Oxygen 6.3 or later)

### Changed
- Updated SDK for Oxygen 6.3

## [6.2.0] - 2022-08-23
### Added
- Support for handling channel timebases with offset (for processing offline files in Oxygen 6.1 or later)

### Changed
- Boost is no longer a compile-time requirement when developing plugins (used only for unit testing)
- Using a C++17 compatible compiler is now a requirement

## [6.1.0] - 2022-05-03
### Added
- Oxygen SDK Manual available in Html and Pdf
- Updated SDK for Oxygen 6.1

### Changed
- Improved Oxygen SDK documentation

## [6.0.0] - 2021-12-21
### Added
- Github CI support with UnitTests to make sure everything builds correctly
- Unit Tests
- Updated SDK for Oxygen 6.0

### Changed
- Updated Examples

## [5.6.0] - 2021-07-02
### Added
- Support for CAN message channels
  - Receive CAN messages
  - Write messages to CAN channel

## [5.5.0] - 2021-03-31
### Added
- Use OXYGEN Plugins in Analysis Mode (Plugins written with the OXYGEN SDK can now be used also offline)

## [5.3.0] - 2020-10-21
### Added
- Export Plugin Support (incl. WAV file example)
- UI Extension Support
- Software Channel Plugins
- Read channel data from Oxygen and forward them to an external interface
- Provide channels with simulated data read from a text file
- Process channel data from Oxygen in an custom calculation and write back the results as channels
