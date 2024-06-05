# Robot Hardware Interface WebSockets API Specification

- [Summary](#summary)
- [Motivation](#motivation)
- [References](#references)
- [Design](#design)
  - [WebSockets Protocol Configuration](#websockets-protocol-configuration)
  - [Text Data Frames](#text-data-frames)
  - [Robot Program Behavior](#robot-program-behavior)
  - [Hardware Behavior](#hardware-behavior)
  - [Hardware Messages](#hardware-messages)
  - [CAN Messages](#can-messages)
  - [RoboRIO Messages](#roborio-messages)
  - [Other Device Messages ("SimDevice")](#other-device-messages-simdevice)

## Summary

A WebSockets API for transmitting robot hardware interface state over a network.  In typical use, one end of the connection is a robot program running in a desktop environment, and the other end of the connection is either a simulation engine, physical robot, or simulation dashboard GUI.

## Motivation

Provide a standard interface for 3rd party software to easily interface with a robot program running on a desktop computer.  Currently this is possible in WPILib only by writing a custom C++ simulation plugin.  Providing a standard text-based network interface to the "hardware" interface layer of a robot program lowers the barrier to entry for this type of development and enables unique capabilities such as direct control of a simple physical robot over a wireless link.

## References

- [RFC 6455](https://tools.ietf.org/html/rfc6455) The WebSocket Protocol
- [RFC 7159](https://tools.ietf.org/html/rfc7159) The JavaScript Object Notation (JSON) Data Interchange Format

## Design

The messages in the protocol are based around typical representations of electronic physical devices, rather than higher level abstractions.  As such, the messages e.g. represent an analog input as a voltage, rather than a “potentiometer” device.  However, the “SimDevice” message can be used to communicate a higher level of abstraction device such as a gyro.  What this means is that simulation engines are responsible for implementing the mapping from their system knowledge (e.g. a joint angle) into an electronic value (e.g. an analog voltage).  See the Trades section for more discussion.

### WebSockets Protocol Configuration

Binary WebSocket frames are not used.  Text WebSocket frames are JSON messages for human readability and ease of debugging.

Both clients and servers shall support unsecure connections (``ws:``) and may support secure connections (``wss:``).  In a trusted network environment (e.g. a robot network), clients that support secure connections should fall back to an unsecure connection if a secure connection is not available.

The resource name for the websockets connection is ``/wpilibws``.  Servers shall reject a second connection to the same resource location as a currently active connection, but may support multiple connections via additional resource names; the resource name is used to prevent duplicate client connections (such as when a web browser is used).  Servers may support multiplexed HTTP file serving on the same port.

The unsecure standard server port number shall be 3300.

### Text Data Frames

Each WebSockets text data frame shall consist of a single JSON object (“message“).

Each message shall be a JSON object with three keys: a ``"type"`` key and lowercase string value describing the type of message, a ``"device"`` key and string value identifying the device, and a ``"data"`` key containing the message data as a JSON object.

The contents of the data object depends on the message type; see the sections for each message for details for the standard message types.  The contents of the data object shall be transmitted as deltas (e.g. the message should only contain the values actually being changed).  Clients and servers are free to ignore data values they don’t find useful, and/or transmit additional data values not specified here.  Clients and servers shall ignore data values they don’t understand.

Data keys have a prefix of either ``"<"``, ``">"``, or ``"<>"``.  This indicates whether the data value is an output from robot code (``"<"``), an input to the robot code (``">"``), or both (``"<>"``).

Clients and servers shall ignore JSON messages that:

* are not objects
* have no ``"type"`` key, ``"device"`` key, or ``"data"`` key
* have a ``"type"`` or ``"device"`` value that is not a string
* have a ``"data"`` value that is not an object
* have a ``"type"`` value that the client or server does not recognize

### Robot Program Behavior

The robot program may operate as either a client or a server.  Generally, the robot program only pays attention to data values with ``">"`` or ``"<>"`` prefixes in received messages.

Upon initial connection, the robot program shall send a message for every initialized device in the program with the current state of that device (both input and output values).  When a robot program removes a device, it shall send a message for that device with ``"<init"`` = false.

For example, if a robot program has an analog input configured for port 1, it will send a message upon initial connection with type ``"AI"``, device ``"1"``, ``"<init"`` true, and ``">voltage"`` set to an indeterminate value.  The remote "hardware" would send messages with type ``"AI"``, device ``"1"``, and ``">voltage"`` set to the (simulated or real) voltage.  When the robot program reads the voltage, it will read the last received ``">voltage"`` value.

The initial state includes joystick and driver station state.

### “Hardware“ Behavior

The “hardware“ (which might be a full-fledged 3D simulation engine, a physical robot, or an interactive GUI) is responsible for mapping the robot program’s inputs and outputs into the real (or virtual) world.  For example, a robot program’s Analog Input 1 might show up as simply ``Analog Input #1`` on a GUI, connect to analog input port #1 on a physical robot, or map to a virtual potentiometer in a 3D simulation engine.

### Hardware Messages

| Type value              | Description                | Device value              |
| ----------------------- | -------------------------- | ------------------------- |
| [``"Accel"``][]         | Accelerometer              | Arbitrary device name     |
| [``"AddressableLED"``][]| Addressable LED Strip      | Arbitrary device number   |
| [``"AI"``][]            | Analog input               | Port index, e.g. "1", "2" |
| [``"AO"``][]            | Analog output              | Port index, e.g. "1", "2" |
| [``"CTREPCM"``][]       | PCM                        | Module number, e.g. "1", "2" |
| [``"DIO"``][]           | Digital input/output       | Port index, e.g. "1", "2" |
| [``"dPWM"``][]          | Duty cycle output          | Arbitrary device number   |
| [``"DriverStation"``][] | Driver station / FMS state | Blank                     |
| [``"DutyCycle"``][]     | Duty cycle input           | Arbitrary device name     |
| [``"Encoder"``][]       | Quadrature encoder         | Arbitrary device number   |
| [``"Gyro"``][]          | Gyro                       | Arbitrary device name     |
| [``"HAL"``][]           | HAL data                   | ``"HAL"``                 |
| [``"Joystick"``][]      | Joystick data              | Joystick number           |
| [``"PWM"``][]           | PWM output                 | Port index, e.g. "1", "2" |
| [``"Relay"``][]         | Relay output               | Port index, e.g. "1", "2" |
| [``"Solenoid"``][]      | Solenoid output            | Module +Port index, e.g. "0,1", "2,5" |

#### Accelerometer ("Accel")

[``"Accel"``]:#accelerometer-accel

A 3-axis accelerometer.

C++/Java implementation note: these are created as either BuiltInAccelerometer or SimDevice nodes where the device name is prefixed by ``"Accel:"``. For example, the device ``"Accel:ADXL362[1]"`` would have a device value of ``ADXL362[1]``.  The BuiltInAccelerometer uses a device name of ``"BuiltInAccel"``.

| Data Key     | Type    | Description                                          |
| ------------ | ------- | ---------------------------------------------------- |
| ``"<init"``  | Boolean | If accelerometer is initialized in the robot program |
| ``"<range"`` | Float   | Desired range in G’s                                 |
| ``">x"``     | Float   | Acceleration in G’s                                  |
| ``">y"``     | Float   | Acceleration in G’s                                  |
| ``">z"``     | Float   | Acceleration in G’s                                  |

#### Addressable LED Strip ("AddressableLED")

[``"AddressableLED"``]:#addressable-led-strip-addressableled

| Data Key           | Type    | Description                                          |
| ------------------ | ------- | ---------------------------------------------------- |
| ``"<init"``        | Boolean | If the led strip is initialized in the robot program |
| ``"<output_port"`` | Integer | DIO pin number                                       |
| ``"<length"``      | Integer | The number of leds in the strip                      |
| ``"<running"``     | Boolean | Whether the strip is outputting data                 |
| ``"<data"``        | Array   | One value per led; value is an object with ``"r"``, ``"g"``, and ``"b"`` keys, representing the rgb (0-255) components of the color |

#### Analog Input ("AI")

[``"AI"``]:#analog-input-ai

The basic analog input just reads a voltage. An analog input can also be configured to use an averaging and oversampling engine.

| Data Key               | Type    | Description                                         |
| ---------------------- | ------- | --------------------------------------------------- |
| ``"<init"``            | Boolean | If analog input is initialized in the robot program |
| ``"<avg_bits"``        | Integer | The number of averaging bits                        |
| ``"<oversample_bits"`` | Integer | The number of oversampling bits                     |
| ``">voltage"``         | Float   | Input voltage, in volts                             |
| ``"<accum_init"``      | Boolean | If the accumulator is initialized in the robot program |
| ``">accum_value"``     | Integer | The accumulated value                               |
| ``">accum_count"``     | Integer | The number of accumulated values                    |
| ``"<accum_center"``    | Integer | The center value of the accumulator                 |
| ``"<accum_deadband"``  | Integer | The accumulator's deadband                          |

#### Analog Output ("AO")

[``"AO"``]:#analog-output-ao

The basic analog output just sends a voltage.

| Data Key              | Type    | Description                                          |
| --------------------- | ------- | ---------------------------------------------------- |
| ``"<init"``           | Boolean | If analog output is initialized in the robot program |
| ``"<voltage"``        | Float   | Output voltage, in volts                             |

#### Digital Input/Output ("DIO")

[``"DIO"``]:#digital-inputoutput-dio

| Data Key      | Type    | Description                                |
| ------------- | ------- | ------------------------------------------ |
| ``"<init"``   | Boolean | If DIO is initialized in the robot program |
| ``"<input"``  | Boolean | True if input, false if output             |
| ``"<>value"`` | Boolean | Input or output state                      |
| ``"<pulse_length"`` | Float | Reserved for future use                |

#### Duty Cycle Output ("dPWM")

[``"dPWM"``]:#duty-cycle-output-dpwm

| Data Key          | Type    | Description                                   |
| ----------------- | ------- | --------------------------------------------- |
| ``"<init"``       | Boolean | If output is initialized in the robot program |
| ``"<duty_cycle"`` | Float   | Duty cycle % (0.0 to 1.0)                     |
| ``"<dio_pin"``    | Integer | DIO pin number                                |

#### Driver Station ("DriverStation")

[``"DriverStation"``]:#driver-station-driverstation

| Data Key          | Type    | Description                                      |
| ----------------- | ------- | ------------------------------------------------ |
| ``">new_data"``   | Boolean | One shot.  If set to true in a message, notifies the robot program that new DS and Joystick data is available. |
| ``">enabled"``    | Boolean | True to enable the robot program |
| ``">autonomous"`` | Boolean | True for autonomous mode; false for teleoperated mode |
| ``">test"``       | Boolean | True for test mode; false for other modes |
| ``">estop"``      | Boolean | True to emergency stop (no motor outputs) |
| ``">fms"``        | Boolean | True if the DS is connected to a Field Management System (FMS) |
| ``">ds"``         | Boolean | True if a DS application is connected |
| ``">station"``    | String  | Station color and number; supported values are ``"red1"``, ``"red2"``, ``"red3"``, ``"blue1"``, ``"blue2"``, ``"blue3"``. |
| ``">match_time"`` | Float   | Match time countdown, in seconds, for each match period (e.g. for 15 second period, starts at 15 and counts down to 0).  If not in a match, -1. |
| ``">game_data"``  | String  | Game-specific data; arbitrary string contents |

#### Duty Cycle Input ("DutyCycle")

[``"DutyCycle"``]:#duty-cycle-input-dutycycle

Duty Cycle inputs are commonly used for absolute encoders.  The position is accumulated over multiple rotations.

C++/Java implementation note: these can be created through the API as SimDevice nodes where the device name is prefixed by ``"DutyCycle:"``. For example, the device ``"DutyCycle:DutyCycleEncoder[1]"`` would have a device value of ``DutyCycleEncoder[1]``.

| Data Key         | Type    | Description                      |
| ---------------- | ------- | -------------------------------- |
| ``">connected"`` | Boolean | True if the encoder is connected |
| ``">position"``  | Float   | The position in rotations        |

#### Quadrature Encoder ("Encoder")

[``"Encoder"``]:#quadrature-encoder-encoder

A relative encoder.  For absolute encoders, use ``"DutyCycle"``.

| Data Key                 | Type    | Description                                         |
| ------------------------ | ------- | --------------------------------------------------- |
| ``"<init"``              | Boolean | If encoder is initialized in the robot program      |
| ``"<channel_a"``         | Integer | Digital channel number for “A” phase                |
| ``"<channel_b"``         | Integer | Digital channel number for “B” phase                |
| ``"<samples_to_avg"``    | Integer | Number of samples to average for period measurement |
| ``">count"``             | Integer | Accumulated count (pulses)                          |
| ``">period"``            | Float   | Period between pulses in seconds                    |
| ``"<reverse_direction"`` | Boolean | If the encoder direction should be inverted         |

#### Gyro ("Gyro")

[``"Gyro"``]:#gyro-gyro

A single axis or 3-axis gyro.  Single axis gyros only use the X angle parameter.

C++/Java implementation note: these can be created created as SimDevice nodes where the device name is prefixed by ``"Gyro:"``. For example, the device ``"Gyro:ADXRS450[1]"`` would have a device value of ``ADXRS450[1]``.

| Data Key          | Type    | Description                                               |
| ----------------- | ------- | --------------------------------------------------------- |
| ``"<init"``       | Boolean | If gyro is initialized in the robot program               |
| ``"<range"``      | Float   | Gyro range in degrees/second (optional)                   |
| ``">connected"``  | Boolean | True if the gyro is connected                             |
| ``">angle_x"``    | Float   | The gyro angle in degrees                                 |
| ``">angle_y"``    | Float   | The gyro angle in degrees                                 |
| ``">angle_z"``    | Float   | The gyro angle in degrees                                 |
| ``">rate_x"``     | Float   | The current gyro angular rate of change in degrees/second |
| ``">rate_y"``     | Float   | The current gyro angular rate of change in degrees/second |
| ``">rate_z"``     | Float   | The current gyro angular rate of change in degrees/second |

#### HAL Data ("HAL")

[``"HAL"``]:#hal-data-hal

Only sent in XRP mode.

Only one of ``">sim_periodic_before"`` or ``">sim_periodic_after"`` should be sent in a single message.

| Data Key                   | Type    | Description                                                    |
| -------------------------- | ------- | -------------------------------------------------------------- |
| ``">sim_periodic_before"`` | Boolean | Sent by the robot before running periodic simulation functions |
| ``">sim_periodic_after"``  | Boolean | Sent by the robot after running periodic simulation functions  |

#### Joystick Data ("Joystick")

[``"Joystick"``]:#joystick-data-joystick

Joystick data is an input to the robot program and should be updated for each input joystick on a periodic basis.  To enable synchronous updates of joystick and driver station state, joystick data is not made visible to the robot program until a DriverStation message with ``">new_data"`` set to true is received.

| Data Key            | Type             | Description |
| ------------------- | ---------------- | --- |
| ``">axes"``         | Array of float   | One array element per axis; value is -1 to 1 range |
| ``">povs"``         | Array of integer | One array element per POV; value is angle in degrees of the POV (e.g. 0, 90, 315) if pressed, or -1 if the POV is not pressed |
| ``">buttons"``      | Array of boolean | One array element per button; true if button is pressed, false if button is released |
| ``"<outputs"``      | Integer          | Bitmask of joystick HID outputs |
| ``"<rumble_left"``  | Float            | Left rumble, value is 0-1 range |
| ``"<rumble_right"`` | Float            | Right rumble, value is 0-1 range |

#### PCM Data ("CTREPCM")

[``"CTREPCM"``]:#pcm-data-ctrepcm

A pneumatic control module is used to regulate the pressure in a pneumatic system by switching a compressor on or off.

| Data Key               | Type    | Description                                |
| ---------------------- | ------- | ------------------------------------------ |
| ``"<init"``            | Boolean | If PCM is initialized in the robot program |
| ``">on"``              | Boolean | Whether the compressor is running          |
| ``"<closed_loop"``     | Boolean | Whether closed-loop control is enabled     |
| ``">pressure_switch"`` | Boolean | The value of the pressure switch           |
| ``">current"``         | Float   | The amount of current being drawn by the compressor, in Amps |

#### PWM Output ("PWM")

[``"PWM"``]:#pwm-output-pwm

PWMs may be used to control either motor controllers or servos.  Typically only one of either ``"<speed"`` (for a motor controller) ``"<position"`` (for a servo), or ``"raw"`` is used for a given PWM.

| Data Key            | Type    | Description                                |
| ------------------- | ------- | ------------------------------------------ |
| ``"<init"``         | Boolean | If PWM is initialized in the robot program |
| ``"<speed"``        | Float   | Speed, -1.0 to 1.0 range                   |
| ``"<position"``     | Float   | Servo position, 0.0 to 1.0 range           |
| ``"<raw"``          | Integer | The pulse time in microseconds             |
| ``"<period_scale"`` | Integer | Scales the PWM signal by squelching setting a 2-bit mask of outputs to squelch (ex. `1` -> squelch every other value; `3` -> squelch 3 of 4 values) |
| ``"<zero_latch"``   | Boolean | Whether the PWM should be latched to 0     |

#### Relay Output ("Relay")

[``"Relay"``]:#relay-output-relay

| Data Key        | Type    | Description                                                    |
| --------------- | ------- | -------------------------------------------------------------- |
| ``"<init_fwd"`` | Boolean | If relay forward direction is initialized in the robot program |
| ``"<init_rev"`` | Boolean | If relay reverse direction is initialized in the robot program |
| ``"<fwd"``      | Boolean | True if forward direction is enabled                           |
| ``"<rev"``      | Boolean | True if reverse direction is enabled                           |

#### Solenoid Output ("Solenoid")

[``"Solenoid"``]:#solenoid-output-solenoid

Solenoids are used to control pneumatic pistons

| Data Key        | Type    | Description                                     |
| --------------- | ------- | ----------------------------------------------- |
| ``"<init"``     | Boolean | If Solenoid is initialized in the robot program |
| ``"<output"``   | Boolean | The state of the solenoid                       |

### CAN Messages

CAN messages all use a device value of ``"DeviceType[Number]"``, where the DeviceType is the vendor-specific CAN device type (motor controller class) name and Number is the CAN device number (the user-visible number passed to the device constructor).

Many of the CAN messages use the same data key/values as other standard messages.  They are separately namespaced to make it easier for implementations to separate them from main robot controller messages.

C++/Java implementation note: these can be created through the API as SimDevice nodes where the device name is prefixed by the message name and ``":"``. For example, ``"CANMotor:Controller[1]"`` would create a device with a type value of ``CANMotor`` and a device value of ``Controller[1]``.

#### CANMotor

Only one of ``"supplyCurrent"`` or ``"motorCurrent"`` should be sent by the hardware; the other value should be set to zero.  If ``"busVoltage"`` is not simulated it should also be set to zero.

| Data Key             | Type             | Description                                                                         |
| -------------------- | ---------------- | ----------------------------------------------------------------------------------- |
| ``"<percentOutput"`` | Integer or Float | Percent output (-1 to 1 range)                                                      |
| ``"<brakeMode"``     | Boolean          | Whether to brake (true) or coast (false) when `\|percentOutput\| < neutralDeadband` |
| `"<neutralDeadband"` | Float            | `\|percentOutput\|` below which `brakeMode` matters (0 to 1)                        |
| ``">supplyCurrent"`` | Float            | The supply current in amps as simulated/measured                                    |
| ``">motorCurrent"``  | Float            | The motor current in amps as simulated/measured                                     |
| ``">busVoltage"``    | Float            | The bus voltage as simulated/measured                                               |

#### CANEncoder

A relative encoder (typically quadrature).  For absolute encoders, use ``"CANDutyCycle"``.

| Data Key        | Type  | Description                      |
| --------------- | ----- | -------------------------------- |
| ``">position"`` | Float | Relative position, in rotations  |
| ``">velocity"`` | Float | Velocity in rotations per second |

#### CANGyro

Uses the same keys as [``"Gyro"``][].

#### CANAccel

Uses the same keys as [``"Accel"``][].

#### CANAIn

Uses the same keys as [``"AI"``][].

The device value may have a suffix for multiple inputs on a single CAN device; ``"-"`` followed by the input name or number is recommended but not required.

#### CANDIO

Uses the same keys as [``"DIO"``][].  This is commonly used for limit switches.

The device value may have a suffix for multiple inputs on a single CAN device; ``"-"`` followed by the input name or number is recommended but not required.

#### CANDutyCycle

Uses the same keys as [``"DutyCycle"``][].

The device value may have a suffix for multiple inputs on a single CAN device; ``"-"`` followed by the input name or number is recommended but not required.

### RoboRIO Messages

These messages are specific to the RoboRIO and will not likely be found in other simulators or hardware devices except in a very limited capacity (e.g. Vin voltage).

| Type value        | Description         | Device value |
| ----------------- | ------------------- | ------------ |
| [``"RoboRIO"``][] | RoboRIO information | Blank        |

#### RoboRIO

[``"RoboRIO"``]:#roborio

The RoboRIO.

| Data Key           | Type    | Description                                 |
| ------------------ | ------- | ------------------------------------------- |
| ``">fpga_button"`` | Boolean | FPGA button state                           |
| ``">vin_voltage"`` | Float   | Vin rail voltage                            |
| ``">vin_current"`` | Float   | Vin rail current                            |
| ``">6v_voltage"``  | Float   | 6V rail voltage                             |
| ``">6v_current"``  | Float   | 6V rail current                             |
| ``">6v_active"``   | Boolean | True if 6V rail active, false if inactive   |
| ``">6v_faults"``   | Integer | Number of faults on 6V rail                 |
| ``">5v_voltage"``  | Float   | 5V rail voltage                             |
| ``">5v_current"``  | Float   | 5V rail current                             |
| ``">5v_active"``   | Boolean | True if 5V rail active, false if inactive   |
| ``">5v_faults"``   | Integer | Number of faults on 5V rail                 |
| ``">3v3_voltage"`` | Float   | 3.3V rail voltage                           |
| ``">3v3_current"`` | Float   | 3.3V rail current                           |
| ``">3v3_active"``  | Boolean | True if 3.3V rail active, false if inactive |
| ``">3v3_faults"``  | Integer | Number of faults on 3.3V rail               |

### Other Device Messages ("SimDevice")

[``"SimDevice"``]:#other-device-messages-simdevice

A device type of ``"SimDevice"`` may be used for extending the protocol for arbitrary complex devices.  The device value is an arbitrary string, generally ``"DeviceName[Port/Index]"``, and the data keys are arbitrary and device-dependent.
