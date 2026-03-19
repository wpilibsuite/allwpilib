# Time Synchronization Protocol Specification, Version 1.0

Protocol Revision 1.0, 08/25/2024

## Background

In a distributed compute environment like robots, time synchronization between computers is increasingly important. Currently, [NetworkTables Version 4.1](https://github.com/wpilibsuite/allwpilib/blob/main/ntcore/doc/networktables4.adoc) provides support for time synchronization of clients with the NetworkTables server using binary PING/PONG messages sent over WebSockets. This approach, while fundamentally the same as is described in this memo, has demonstrated some opportunities for improvement:

- PING/PONG messages are processed in the same queue as other NetworkTables messages. Depending on the underlying implementation and processor speed, this can incur message processing delays and increase client-calculated Round-Trip Time (RTT), and cause messages to arrive at the server timestamped in the future.
- Messages use WebSockets over TCP for their transport layer. We don't need the robustness guarantees of TCP as our connection is stateless.

For these reasons, a time synchronization solution separate from NetworkTables communication was desired. Architecture decisions made to address these issues are:

- Use the User Datagram Protocol (UDP) transport layer, as we don't need the robustness guarantees afforded by TCP. As a Client, if a PING isn't replied to, we'll just try again at the start of the next PING window. As a bonus, we are free to use UDP port 5810 as NetworkTables only uses TCP Port 5810/5811 as of Version 4.1.
- Use a separate thread from the current NetworkTables libUV runner.


## Prior Art

The [NetworkTables 4.1 timestamp synchronization](https://github.com/wpilibsuite/allwpilib/blob/main/ntcore/doc/networktables4.adoc#timestamps) approach, an implementation of [Cristian's Algorithm](https://en.wikipedia.org/wiki/Cristian%27s_algorithm). We also implement Cristian’s Algorithm.

The [Precision Time Protocol](https://en.wikipedia.org/wiki/Precision_Time_Protocol#Synchronization) at it's core does something similar with Sync/Delay_Req/Delay_Resp. We do not have (guaranteed) access to hardware timestamping, but we utilize this PING/PONG pattern to estimate total round-trip time.


## Roles

```{graphviz}
digraph CristianAlgorithm {
    ratio=0.5;
    bgcolor="transparent";

    node [
        fontcolor = "#e6e6e6",
        style = filled,
        color = "#e6e6e6",
        fillcolor = "#333333"
        fontsize=10;
    ]

    edge [
        color = "#e6e6e6",
        fontcolor = "#e6e6e6"
        fontsize=10;
    ]

    rankdir=LR;
    node [shape=box, style=filled, color=lightblue];

    user_send [label="User Sends T1"];
    server_receive [label="Server Receives T1"];
    server_send [label="Server Sends T2"];
    user_receive [label="User Receives T2"];
    user_compute [label="User Computes Time"];

    user_send -> server_receive [label="T1 (Request)"];
    server_receive -> server_send [label="T1 received by server"];
    server_send -> user_receive [label="T2 sent by server"];
    user_receive -> user_compute [label="T2 received by user"];
    user_compute -> user_send [label="Computed Time: T3 = T2 + (deltaT2 - deltaT1)/2"];
}
```

Time Synchronization Protocol (TSP) participants can assume either a server role or a client role. The server role is responsible for listening for incoming time synchronization requests from clients and replying appropriately. The client role is responsible for sending "Ping" messages to the server and listening for "Pong" replies to estimate the offset between the server and client time bases.

All time values shall use units of microseconds. The epoch of the time base this is measured against is unspecified.

Clients shall periodically (e.g. every few seconds) send, in a manner that minimizes transmission delays, a **TSP Ping Message** that contains the client's current local time.

When the server receives a **TSP Ping Message** from any client, it shall respond to the client, in a manner that minimizes transmission delays, with a **TSP Pong message** encoding a timestamp of its (the server's) current local time (in microseconds), and the client-provided data value.

When the client receives a **TSP Pong Message** from the server, it shall verify that the `Client Local Time` corresponds to the currently in-flight TSP Ping message; if not, it shall drop this packet. The round trip time (RTT) shall be computed from the delta between the message's data value and the current local time.  If the RTT is less than that from previous measurements, the client shall use the timestamp in the message plus ½ the RTT as the server time equivalent to the current local time, and use this equivalence to compute server time base timestamps from local time for future messages.

## Transport

Communication between server and clients shall occur over the User Datagram Protocol (UDP) Port 5810.

## Message Format

The message format forgoes CRCs (as these are provided by the Ethernet physical layer) or packet delineation (as our packets are assumed be under the network MTU). **TSP Ping** and **TSP Pong** messages shall be encoded in a manor compatible with a WPILib packed struct with respect to byte alignment and endianness.

### TSP Ping

| Offset | Format | Data | Notes |
| ------ | ------ | ---- | ----- |
| 0 | uint8 | Protocol version | This field shall always set to 1 (0b1) for TSP Version 1. |
| 1 | uint8 | Message ID | This field shall always be set to 1 (0b1). |
| 2 | uint64 | Client Local Time | The client's local time value, at the time this Ping message was sent. |

### TSP Pong

| Offset | Format | Data | Notes |
| ------ | ------ | ---- | ----- |
| 0 | uint8 | Protocol version | This field shall always set to 1 (0b1) for TSP Version 1.
| 1 | uint8 | Message ID | This field shall always be set to 2 (0b2).
| 2 | uint64 | Client Local Time | The client's local time value from the Ping message that this Pong is generated in response to.
| 10 | uint64 | Server Local Time | The current time at the server, at the time this Pong message was sent.


## Optional Protocol Extensions

Clients may publish statistics to NetworkTables. If they do, they shall publish to a key that is globally unique per participant in the Time Synchronization network. If a client implements this, it shall provide the following publishers:

| Key | Type | Notes |
| ------ | ------ | ---- |
| offset_us | Integer | The time offset that, when added to the client's local clock, provides server time |
| ping_tx_count | Integer | The total number of TSP Ping packets transmitted |
| ping_rx_count | Integer | The total number of TSP Ping packets received |
| pong_rx_time_us | Integer | The time, in client local time, that the last pong was received |
| rtt2_us | Integer | The time in us from last complete (ping transmission to pong reception) |

PhotonVision has chosen to publish to the sub-table `/photonvision/.timesync/{DEVICE_HOSTNAME}`. Future implementations of this protocol may decide to implement this as a structured data type.

## Wireshark Dissector

![](images/wireshark.jpg)

A [WireShark dissector](https://raw.githubusercontent.com/PhotonVision/photonvision/refs/heads/main/devTools/photon.lua) created for Wireshark ~=4.6 can be used to inspect Time Syncronization messages. Copy the dissector to your Wireshark plugin directory (for me, that's `C:\Users\Me\AppData\Roaming\Wireshark\plugins`), and open the capture. Because TSP uses UDP Unicast, data must be collected on the coprocessor or robot processor using a command similar to:

```
sudo tcpdump -i any port 5810 -w tsp_capture.pcap
```
