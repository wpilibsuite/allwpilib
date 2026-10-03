import time

from ntcore import NetworkTableInstance


def test_server_client_protocol_version(tmp_path):
    server = NetworkTableInstance.create()
    client = NetworkTableInstance.create()
    try:
        server.start_server(str(tmp_path / "nt.json"), "127.0.0.1", "", 10033)
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
    finally:
        NetworkTableInstance.destroy(client)
        NetworkTableInstance.destroy(server)
