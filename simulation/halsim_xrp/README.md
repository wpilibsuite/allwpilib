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

The native Bluetooth packet transport prefers LE L2CAP Credit-Based Mode on Linux, with GATT fallback. Windows and macOS use GATT Write Without Response and notifications. CoreBluetooth exposes L2CAP as a stream, which does not preserve the packet boundaries required by this protocol.

GATT connections must support at least 85 bytes per notification (ATT MTU 88). The client checks this before reporting a connection. Windows and macOS manage MTU negotiation; Linux requests an MTU large enough for the configured packet capacity.

Periodic control packets are best effort and are not retried when the transport is busy. macOS submits these writes without waiting for CoreBluetooth write readiness; Windows submits them without waiting for earlier WinRT writes to complete. A one-shot rename request is retained until the transport is ready (after outstanding writes complete on Windows); newer control packets are dropped while it is pending so they cannot make the rename's sequence stale. The firmware replies to the rename with an ACK-only status packet that reports whether the name was saved.

### Linux connection diagnostics

Set `WPI_BLUETOOTH_DEBUG=1` before starting the simulator to enable Bluetooth
client diagnostics on stderr. For example, prefix your normal launch command:

```sh
WPI_BLUETOOTH_DEBUG=1 ./your-simulator-command 2>&1 | tee /tmp/xrp-client.log
```

The Linux transport logs UTC timestamps, a client identifier, connection
generations, socket descriptors and CIDs, `connect()`/`SO_ERROR` results,
L2CAP-to-GATT fallback, BlueZ disconnect retry results, and GATT discovery stages.
A GATT timeout includes the stage still awaiting a response. Successful discovery
reports the negotiated MTU and characteristic/notification descriptor handles.

Traffic diagnostics log the first transmit and receive packet on each socket,
then summarize counts at most once every five seconds while packets are flowing.
The first transmit/receive can also trigger a summary. Counts reset when a new
socket opens; `rx_age_ms=-1` means no status packet has arrived on that socket.
Sends count packets accepted by the local socket, not delivery acknowledgments.
No control or status payload contents are logged. Diagnostics are off by default.

For reconnect problems, capture this output alongside the device's USB Serial
log and a Bluetooth monitor capture:

```sh
sudo btmon -C 200 -P -T -w /tmp/xrp-reconnect.btsnoop 2>&1 | tee /tmp/xrp-reconnect.btmon.txt
```

Include a successful connection, the disconnect, and a failing reconnect before
resetting the device. The explicit width avoids a narrow-output formatting crash
in older btmon versions; BlueZ fixes the bounds check in
[commit dca0f10fd560](https://kernel.googlesource.com/pub/scm/bluetooth/bluez/+/dca0f10fd560f8095493021ca2ac20236fde026e).

If the kernel journal reports `Bluetooth: Unable to allocate ident: -28` and
btmon shows L2CAP signaling requests with identifier zero, check for the Linux
kernel identifier leak fixed by
[commit 6e1930ece855](https://github.com/torvalds/linux/commit/6e1930ece855a4c256f1c7e6632d634cfb9888b5).
On affected kernels, receiving status packets eventually exhausts signaling
identifiers and reconnects can time out despite replies from the device. These
are kernel signaling identifiers, separate from XRP packet sequence numbers.
Use a kernel containing the fix when evaluating L2CAP reconnect reliability.

An immediate Linux GATT `connect()` failure with `ENOMEM` can also occur when
Linux refuses a channel on an underlying connection marked for teardown. The
client waits 250 ms and retries this specific failure once; disconnecting or
starting another connection cancels the pending retry. A second failure is
reported normally. This is separate from GATT discovery or firmware allocation.

If btmon shows repeated classic `Inquiry` commands during an LE connection
attempt, check for other discovery clients. The XRP scanner requests LE-only
discovery. Classic inquiry can delay the controller's LE connection setup beyond
the client's connection timeout. KDE Connect 25.12.3 can leave an existing
scan alive after its Bluetooth backend reports disabled: its
[`disable()` implementation](https://github.com/KDE/kdeconnect-kde/blob/v25.12.3/core/backends/bluetooth/bluetoothlinkprovider.cpp#L80)
drops the discovery-agent pointer without stopping it. That version also
[checks `AsyncLinkProvider` at startup](https://github.com/KDE/kdeconnect-kde/blob/v25.12.3/core/daemon.cpp#L81),
while the CLI stores `BluetoothLinkProvider`. Repeating the disable command or
restarting alone may therefore leave scanning enabled. For this version, append
`AsyncLinkProvider` to the existing `[General] disabled_providers` list in
`~/.config/kdeconnect/config`, retaining `BluetoothLinkProvider` and other entries,
and restart KDE Connect. Verify discovery stays off beyond its 30-second scan
interval; do not rely only on the CLI's reported state. Remove both Bluetooth
entries and restart when intentionally re-enabling that backend. Check the
adapter's actual discovery state with:

```sh
busctl --system get-property org.bluez /org/bluez/hci0 org.bluez.Adapter1 Discovering
```

To identify other discovery clients, capture BlueZ adapter calls (requires
system-bus monitoring privileges) and match each `sender=:1.N` to
`busctl --system list`. Start the capture before triggering a fresh discovery
request: an existing session can keep scanning without any further D-Bus calls,
so a quiet trace alone does not establish that discovery has stopped.

```sh
sudo timeout 20s dbus-monitor --system "type='method_call',destination='org.bluez',interface='org.bluez.Adapter1'" > /tmp/xrp-discovery-dbus.log
busctl --system list --no-pager > /tmp/xrp-discovery-clients.txt
```

### macOS application permissions

The application hosting HALSim XRP must provide `NSBluetoothAlwaysUsageDescription` in its `Info.plist`, for example:

```xml
<key>NSBluetoothAlwaysUsageDescription</key>
<string>Connect to an XRP robot to run WPILib robot programs.</string>
```

The WPILib CMake application bundle template includes this entry. A simulation extension is a library; its own property list cannot supply permissions for a separate host executable. For Java, Python, and IDE launches, ensure the host application/launcher provides the entry and has Bluetooth access in System Settings. macOS can terminate an application that accesses Bluetooth without the required usage description. Verify the actual launcher used for simulation, as its permissions may differ from those of Terminal.

## XRP Protocol

The WPILib XRP protocol is binary-based to save Bluetooth bandwidth. Each GATT write value, GATT notification value, or L2CAP SDU contains exactly one protocol packet with no additional length prefix.

All multi-byte values are big-endian. Each packet starts with the same 5-byte header:

| 2 bytes             | 1 byte            | 2 bytes                | n bytes   |
|---------------------|-------------------|------------------------|-----------|
| _uint16_t_ sequence | _uint8_t_ control | _uint16_t_ field mask  | payload   |

The payload contains each field selected by the field mask, emitted in ascending bit order. Packets with unknown field bits or payload sizes that do not exactly match the selected fields are ignored.

Sequences advance modulo 65536. Duplicate and stale packets are ignored; forward jumps must be smaller than 32768. Status packets may be coalesced by the firmware, including across rollover. A new connection resets the client's status sequence tracking.

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
| 11    | Command ACK  | _uint16_t_ control sequence, _uint16_t_ control field mask, _uint8_t_ result |

XRP status currently reports DIO 0, the user button. Analog values are scaled over `0` to `5 V`, where `0` is `0 V` and `65535` is `5 V`. Command ACK status packets are sent with only bit 11 set; result `0` is success and result `1` is rejected.

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
