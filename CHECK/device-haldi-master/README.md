# README
## Installing CCS, SDK
- Download and install the latest [CCS](https://www.ti.com/tool/download/CCSTUDIO).  The version is currently 10.1.0.00010.
- Alongside, install the [SDK](https://www.ti.com/tool/download/SIMPLELINK-CC2640R2-SDK).  The current version with all BLE5 features is 4.20.00.04.
- (As newer versions of CCS and the SDK become available, we will test, upgrade and commit.)

## Building
- Open CCS, point at this directory for the workspace.
- Import projects into the workspace: both the `_app` and the `_stack`.

# Project
This is an adaptation of the `ProjectZero` example.
- Unnecessary services: LED, data, buttons have been removed.
- A Sydäntek service has been added.
- The device identification and advertisement data have been updated.

## Service and Characteristics
In keeping with Bluetooth SIG guidelines, unique identifiers were generated.
- The base UUID is `a965db41-5e30-ad9e-fe47-02a582287800`.  This is used for the service.
- Other UUID used by characteristics in the service are: parameters (`...01`); and notification
(`...02`).

## Operation
The device is intended for use by a single connected central at any point.
- Upon connection, the central (mobile phone) initializes the parameters by writing to them: #leads, the sampling frequency for limb and chest leads.  (This is done at _every_ (re-) connection.)
- At this point, the device starts notifying with characteristic value changes.  Enable notification to receive them.

## Protocol
- Attribute values are 244 bytes long (or can be less)
- The first four bytes are a (little-endian) sequence number.  Missing updates can thus be detected.
- Remaining bytes are sequences of samples, #leads * 3 bytes each, little-endian.  With data collected from 8 leads, there are 10 samples in a notification; with 2 leads, 40; with 1 lead, 80 samples.  Note that the duration covered will depend upon the sampling rate.

## TODO
- Timer based on `sysbios` instead of `util`: for a higher sampling frequency than 1000Hz, and also directly used without task switching.
- Simpler interface to collect data over SPI.  Code dealing with this needs to be ported over.
