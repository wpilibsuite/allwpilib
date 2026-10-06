# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

import json
import time

from ntcore import NetworkTableInstance


def test_persistent_property_updates_saved_without_value_changes(tmp_path):
    persistent_file = tmp_path / "persistent.json"
    server = NetworkTableInstance.create()
    topic = server.get_integer_topic("/test")
    pub = topic.publish()

    def wait_for_saved(properties):
        expected = [
            {"name": "/test", "type": "int", "value": 42, "properties": properties}
        ]
        deadline = time.monotonic() + 5
        data = None
        while time.monotonic() < deadline:
            try:
                data = json.loads(persistent_file.read_text())
            except (FileNotFoundError, json.JSONDecodeError):
                data = None
            if data == expected:
                return
            time.sleep(0.01)
        assert data == expected

    try:
        server.start_server(str(persistent_file), "127.0.0.1", "", 0)
        pub.set(42)
        topic.set_persistent(True)
        server.flush_local()
        wait_for_saved({"persistent": True})

        topic.set_property("unit", "m")
        server.flush_local()
        wait_for_saved({"persistent": True, "unit": "m"})

        topic.delete_property("unit")
        server.flush_local()
        wait_for_saved({"persistent": True})
    finally:
        pub.close()
        NetworkTableInstance.destroy(server)
