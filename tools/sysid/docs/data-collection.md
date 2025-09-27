# Data Collection

This document details how data must be sent over NetworkTables for accurate data collection. Note that the data format has changed from what the old [frc-characterization](https://github.com/wpilibsuite/frc-characterization) tool used to generate.

## NetworkTables Data Entries

Here is a list of the NT entries that are used to send and collect data between sysid and the robot program:

|               NT Entry                |   Type   |           Description                                                                                                                                        |
| --------------------------------------| -------- | -------------------------------------------------------------------------------------------------------------------------------------------------- |
| `/SmartDashboard/SysIdTelemetry`      | `string` | Used to send telemetry from the robot program. This data is sent after the test completes once the robot enters the disabled state.  |
| `/SmartDashboard/SysIdVoltageCommand` | `double` | Used to either send the ramp rate (V/s) for the quasistatic test or the voltage (V) for the dynamic test.  |
| `/SmartDashboard/SysIdTestType`       | `string` | Used to send the test type ("Quasistatic" or "Dynamic") which helps determine how the `VoltageCommand` entry will be used.  |
| `/SmartDashboard/SysIdRotate`         | `bool`   | Used to receive the rotation bool from the Logger. If this is set to true, the drivetrain will rotate. It is only applicable for drivetrain tests.  |

## Telemetry Format

There are two formats used to send telemetry from the robot program. One format is for non-drivetrain mechanisms, whereas the other is for all drivetrain tests (linear and angular). All timestamps must be in seconds.

### Non-Drivetrain Mechanisms

`timestamp, voltage, position, velocity`

Example JSON:

```json
{
"fast-backward": [
[
timestamp 1,
voltage 1,
position 1,
velocity 1
],
[
timestamp 2,
voltage 2,
position 2,
velocity 2
]
],
"fast-forward": [
[
timestamp 1,
voltage 1,
position 1,
velocity 1
],
[
timestamp 2,
voltage 2,
position 2,
velocity 2
]
],
"slow-backward": [
[
timestamp 1,
voltage 1,
position 1,
velocity 1
],
[
timestamp 2,
voltage 2,
position 2,
velocity 2
]
],
"slow-forward": [
[
timestamp 1,
voltage 1,
position 1,
velocity 1
],
[
timestamp 2,
voltage 2,
position 2,
velocity 2
]
],
"sysid": true,
"test": "Simple",
"units": "Rotations",
"unitsPerRotation": 1.0
}
```

Supported test types for the "test" field in this data format include "Arm",
"Elevator", and "Simple". Supported unit types include "Meters", "Feet",
"Inches", "Radians", "Rotations", and "Degrees".

### Drivetrain

`timestamp, l voltage, r voltage, l position, r position, l velocity, r velocity, angle, angular rate`

Note that all positions and velocities should be in rotations of the output and rotations/sec of the output respectively. If there is a gearing between the encoder and the output, that should be taken into account.

Example JSON:

```json
{
"fast-backward": [
[
timestamp 1,
l voltage 1,
r voltage 1,
l position 1,
r position 1,
l velocity 1,
r velocity 1,
angle 1,
angular rate 1
],
[
timestamp 2,
l voltage 2,
r voltage 2,
l position 2,
r position 2,
l velocity 2,
r velocity 2,
angle 2,
angular rate 2
]
],
"fast-forward": [
[
timestamp 1,
l voltage 1,
r voltage 1,
l position 1,
r position 1,
l velocity 1,
r velocity 1,
angle 1,
angular rate 1
],
[
timestamp 2,
l voltage 2,
r voltage 2,
l position 2,
r position 2,
l velocity 2,
r velocity 2,
angle 2,
angular rate 2
]
],
"slow-backward": [
[
timestamp 1,
l voltage 1,
r voltage 1,
l position 1,
r position 1,
l velocity 1,
r velocity 1,
angle 1,
angular rate 1
],
[
timestamp 2,
l voltage 2,
r voltage 2,
l position 2,
r position 2,
l velocity 2,
r velocity 2,
angle 2,
angular rate 2
]
],
"slow-forward": [
[
timestamp 1,
l voltage 1,
r voltage 1,
l position 1,
r position 1,
l velocity 1,
r velocity 1,
angle 1,
angular rate 1
],
[
timestamp 2,
l voltage 2,
r voltage 2,
l position 2,
r position 2,
l velocity 2,
r velocity 2,
angle 2,
angular rate 2
]
],
"sysid": true,
"test": "Drivetrain",
"units": "Rotations",
"unitsPerRotation": 1.0
}
```

Supported test types for the "test" field in this data format include
"Drivetrain" and "Drivetrain (Angular)". Supported unit types include "Meters",
"Feet", "Inches", "Radians", "Rotations", and "Degrees".
