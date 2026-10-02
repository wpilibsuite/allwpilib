# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

from http.client import HTTPConnection
import time


def test_outline_viewer(nt_server):
    nt_server.start_test()
    deadline = time.monotonic() + 3
    while True:
        conn = HTTPConnection("127.0.0.1", nt_server.port, timeout=3)
        try:
            conn.request("GET", "/")
            break
        except ConnectionRefusedError:
            conn.close()
            if time.monotonic() >= deadline:
                raise
            time.sleep(0.01)

    try:
        response = conn.getresponse()
        assert response.status == 200
        assert response.getheader("Content-Type") == "text/html; charset=utf-8"
        body = response.read().decode("utf-8")
        assert body.startswith("<!doctype html>")
        assert "NetworkTables Outline Viewer" in body
        assert "v4.1.networktables.first.wpi.edu" in body
        assert "</html>" in body
    finally:
        conn.close()
