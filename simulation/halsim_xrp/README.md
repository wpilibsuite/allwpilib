# HAL XRP Client

This is an extension that provides a client version of the XRP protocol for transmitting robot hardware interface state to an XRP robot over Bluetooth LE.

## Configuration

The XRP client can be configured through the XRP Bluetooth window in the simulator GUI. The window can scan for nearby XRP Bluetooth LE devices on supported platforms and pair with the selected target where the platform exposes explicit pairing.

The client also has environment variable configuration for headless use.

``HALSIMXRP_BT_ADDRESS``: The Bluetooth target to connect to. On Linux and Windows, this is the Bluetooth device address. On macOS, this can be the CoreBluetooth UUID or advertised XRP device name. No default.

``HALSIMXRP_BT_ADDRESS_TYPE``: The Bluetooth LE address type. Supported values are `public` and `random`. Defaults to `random`.

The firmware advertises a device name of the form `WPIXRP-AAAA-BBBB`, exposes a custom GATT packet service, and accepts LE L2CAP Credit-Based Mode connections on PSM `0x0081`. Each GATT write value, GATT notification value, or L2CAP SDU contains one XRP protocol packet.

GATT service UUID: `7d2ea28a-f7bd-485d-9d6a-2c3f0b214a3f`

GATT control characteristic UUID: `7d2ea28b-f7bd-485d-9d6a-2c3f0b214a3f`

GATT status characteristic UUID: `7d2ea28c-f7bd-485d-9d6a-2c3f0b214a3f`

The native Bluetooth packet transport prefers LE L2CAP Credit-Based Mode on Linux and macOS. Linux and macOS fall back to GATT if the L2CAP channel cannot be opened. Windows uses GATT Write Without Response and notifications.

## XRP Protocol

The WPILib XRP protocol is binary-based to save Bluetooth bandwidth. Each GATT write value, GATT notification value, or L2CAP SDU contains exactly one protocol packet with no additional length prefix.

All multi-byte values are big-endian. Each packet starts with the same 5-byte header:

| 2 bytes             | 1 byte            | 2 bytes                | n bytes   |
|---------------------|-------------------|------------------------|-----------|
| _uint16_t_ sequence | _uint8_t_ control | _uint16_t_ field mask  | payload   |

The payload contains each field selected by the field mask, emitted in ascending bit order. Packets with unknown field bits or payload sizes that do not exactly match the selected fields are ignored.

### Control Byte

For WPILib -> XRP control packets, the control byte indicates the current `enabled` state of the WPILib robot code. When this is set to `1`, the robot is enabled, and when it is set to `0`, it is disabled.

For XRP -> WPILib status packets, the control byte is a copy of the most recent accepted WPILib -> XRP control byte.

### WPILib -> XRP Control Fields

| Bit   | Field       | Payload       |
|-------|-------------|---------------|
| 0     | XRPMotor 0  | _int16_t_ pwm |
| 1     | XRPMotor 1  | _int16_t_ pwm |
| 2     | XRPMotor 2  | _int16_t_ pwm |
| 3     | XRPMotor 3  | _int16_t_ pwm |
| 4     | XRPServo 4  | _uint8_t_ degrees |
| 5     | XRPServo 5  | _uint8_t_ degrees |
| 6     | XRPServo 6  | _uint8_t_ degrees |
| 7     | XRPServo 7  | _uint8_t_ degrees |
| 8     | DIO 0-7     | _uint8_t_ present mask, _uint8_t_ value mask |
| 15    | Device name | _uint8_t_ length, followed by `length` printable ASCII bytes |

XRPMotor `pwm` values are clamped to `-255` to `255`, matching the XRP motor PWM magnitude plus direction. XRPServo `degrees` values are clamped to `0` to `180`, matching the integer degree value applied by the XRP servo library. DIO payload bits are channel-indexed; bit `n` in the present mask means DIO channel `n` is included, and bit `n` in the value mask is that channel's value.

A device name control packet must use only field bit 15. The payload may contain either the full `WPIXRP-` name or just the suffix. The firmware validates the name, writes it to `/config.ini`, and reboots so the new Bluetooth advertisement name is applied.

### XRP -> WPILib Status Fields

| Bit   | Field        | Payload |
|-------|--------------|---------|
| 0     | Encoder 0    | _int32_t_ count, _uint32_t_ period numerator |
| 1     | Encoder 1    | _int32_t_ count, _uint32_t_ period numerator |
| 2     | Encoder 2    | _int32_t_ count, _uint32_t_ period numerator |
| 3     | Encoder 3    | _int32_t_ count, _uint32_t_ period numerator |
| 4     | DIO 0-7      | _uint8_t_ present mask, _uint8_t_ value mask |
| 5     | XRPGyro      | _float_ rate_x, _float_ rate_y, _float_ rate_z, _float_ angle_x, _float_ angle_y, _float_ angle_z |
| 6     | XRPAccel     | _float_ accel_x, _float_ accel_y, _float_ accel_z |
| 7     | AnalogIn 0   | _uint16_t_ value |
| 8     | AnalogIn 1   | _uint16_t_ value |
| 9     | AnalogIn 2   | _uint16_t_ value |
| 10    | Timing       | _uint16_t_ last control sequence, _uint16_t_ control receive age in 10 us units |

XRP status currently reports DIO 0, the user button. Analog values are scaled over `0` to `5 V`, where `0` is `0 V` and `65535` is `5 V`.

#### Encoders

Encoder IDs map to XRP encoders as follows:

| ID | Description |
|----|-------------|
| 0  | Left Motor Encoder  |
| 1  | Right Motor Encoder |
| 2  | Motor 3 Encoder     |
| 3  | Motor 4 Encoder     |

Encoder period uses a fixed denominator of `1000000`. `period numerator >> 1` is the period in microseconds, and the low bit is the direction bit (`1` for forward, `0` for reverse). A period numerator of `0xffffffff` indicates no valid period.

#### Timing

The timing field lets the client estimate application-level round-trip latency.

`last control sequence` echoes the most recent accepted WPILib -> XRP control packet sequence number. `control receive age in 10 us units * 10` is the time in microseconds between receiving that control packet and producing the status packet.

A control receive age of `0xffff` indicates no control packet has been accepted yet or the age exceeded the representable range.
