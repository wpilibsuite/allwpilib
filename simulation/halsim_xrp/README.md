# HAL XRP Client

This is an extension that provides a client version of the XRP protocol for transmitting robot hardware interface state to an XRP robot over UDP.

## Configuration

The XRP client has a number of configuration options available through environment variables.

``HALSIMXRP_HOST``: The host to connect to.  Defaults to localhost.

``HALSIMXRP_PORT``: The port number to connect to.  Defaults to 3540.

## XRP Protocol

The WPILib -> XRP protocol is binary-based to save on bandwidth due to hardware limitations of the XRP robot. The messages to/from the XRP follow a the format below:

| 2 bytes             | 1 byte            | n bytes                             |
|---------------------|-------------------|-------------------------------------|
| _uint16_t_ sequence | _uint8_t_ control | [&lt;Tagged Data&gt;](#tagged-data) |

### Control Byte

The control byte is used to indicate the current `enabled` state of the WPILib robot code. When this is set to `1`, the robot is enabled, and when it is set to `0` it is disabled.

Messages originating from the XRP have an unspecified value for the control byte.

### Tagged Data

The `Tagged Data` section can contain an arbitrary number of data blocks. Each block has the format below:

| 1 byte         | 1 byte          | n bytes         |
|----------------|-----------------|-----------------|
| _uint8_t_ size | _uint8_t_ tagID | &lt;payload&gt; |

The `size` byte encodes the size of the data block, _excluding_ itself. Thus the smallest block size is 2 bytes, with a size value of 1 (1 size byte, 1 tag byte, 0 payload bytes). Maximum size of the payload is 254 bytes.


Utilizing tagged data blocks allows us to send multiple pieces of data in a single UDP packet. The tags currently implemented for the XRP are as follows:

| Tag  | Description                   |
|------|-------------------------------|
| 0x12 | [XRPMotor](#xrpmotor)         |
| 0x13 | [XRPServo](#xrpservo)         |
| 0x14 | [DIO](#dio)                   |
| 0x15 | [AnalogIn](#analogin)         |
| 0x16 | [XRPGyro](#xrpgyro)           |
| 0x17 | [BuiltInAccel](#builtinaccel) |
| 0x18 | [Encoder](#encoder)           |


#### XRPMotor

| Order | Data Type | Description       |
|-------|-----------|-------------------|
| 0     | _uint8_t_ | ID                |
| 1     | _float_   | Value [-1.0, 1.0] |

IDs:
| ID | Description |
|----|-------------|
| 0  | Left Motor  |
| 1  | Right Motor |
| 2  | Motor 3     |
| 3  | Motor 4     |

#### XRPServo

| Order | Data Type | Description      |
|-------|-----------|------------------|
| 0     | _uint8_t_ | ID               |
| 1     | _float_   | Value [0.0, 1.0] |

IDs:
| ID | Description |
|----|-------------|
| 4  | Servo 1     |
| 5  | Servo 2     |

#### DIO

| Order | Data Type | Description        |
|-------|-----------|--------------------|
| 0     | _uint8_t_ | ID                 |
| 1     | _uint8_t_ | Value (True/False) |

#### AnalogIn

| Order | Data Type | Description |
|-------|-----------|-------------|
| 0     | _uint8_t_ | ID          |
| 1     | _float_   | Value       |

#### XRPGyro

| Order | Data Type | Description   |
|-------|-----------|---------------|
| 0     | _float_   | rate_x (dps)  |
| 1     | _float_   | rate_y (dps)  |
| 2     | _float_   | rate_z (dps)  |
| 3     | _float_   | angle_x (deg) |
| 4     | _float_   | angle_y (deg) |
| 5     | _float_   | angle_z (deg) |

#### BuiltInAccel

| Order | Data Type | Description |
|-------|-----------|-------------|
| 0     | _float_   | accel_x (g) |
| 1     | _float_   | accel_y (g) |
| 2     | _float_   | accel_z (g) |

#### Encoder

| Order | Data Type  |     Description    |
|-------|------------|--------------------|
| 0     | _uint8_t_  | ID                 |
| 1     | _int32_t_  | Count              |
| 2     | _uint32_t_ | Period Numerator   |
| 3     | _uint32_t_ | Period Denominator |

IDs:
| ID | Description         |
|----|---------------------|
| 0  | Left Motor Encoder  |
| 1  | Right Motor Encoder |
| 2  | Motor 3 Encoder     |
| 3  | Motor 4 Encoder     |
