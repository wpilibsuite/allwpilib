import socket
import struct
import time

from ntcore import NetworkTableInstance, _now


def test_server_client_protocol_version(tmp_path):
    server = NetworkTableInstance.create()
    client = NetworkTableInstance.create()
    try:
        server.start_server(str(tmp_path / "nt.json"), " 127.0.0.1 ", "", 10033)
        client.set_server("127.0.0.1", 10033)
        client.start_client("timesync-test")
        deadline = time.monotonic() + 5
        while (
            not server.get_connections() or not client.is_connected()
        ) and time.monotonic() < deadline:
            time.sleep(0.05)
        server_connections = server.get_connections()
        client_connections = client.get_connections()
        assert len(server_connections) == 1
        assert len(client_connections) == 1
        assert server_connections[0].protocol_version == 0x0402
        assert client_connections[0].protocol_version == 0x0402
        deadline = time.monotonic() + 5
        while client.get_server_time_offset() is None and time.monotonic() < deadline:
            time.sleep(0.05)
        assert client.get_server_time_offset() is not None
    finally:
        NetworkTableInstance.destroy(client)
        NetworkTableInstance.destroy(server)


def test_server_wire_timestamps_nanoseconds(tmp_path):
    server = NetworkTableInstance.create()
    try:
        server.start_server(str(tmp_path / "nt.json"), "127.0.0.1", "", 10036)
        with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as peer:
            peer.settimeout(2)
            client_time_ns = 123_456_789
            before = _now()
            peer.sendto(struct.pack("<BBQ", 1, 1, client_time_ns), ("127.0.0.1", 10036))
            response = peer.recv(19)
            after = _now()
            version, message_id, echoed_time, server_time_ns = struct.unpack(
                "<BBQQ", response
            )
            assert (version, message_id, echoed_time) == (1, 2, client_time_ns)
            assert before <= server_time_ns <= after
    finally:
        NetworkTableInstance.destroy(server)
