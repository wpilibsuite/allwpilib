# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

import http.client
import json
import socket
import time

import pytest

from ntcore import NetworkTableInstance


@pytest.fixture
def rest_server(tmp_path):
    # Choose an unused loopback port for this server instance.
    with socket.socket() as sock:
        sock.bind(("127.0.0.1", 0))
        port = sock.getsockname()[1]
    inst = NetworkTableInstance.create()
    inst.start_server(str(tmp_path / "persistent.json"), "127.0.0.1", "", port)
    try:
        deadline = time.monotonic() + 5
        while True:
            try:
                with socket.create_connection(("127.0.0.1", port), timeout=1):
                    break
            except OSError:
                if time.monotonic() >= deadline:
                    raise
                time.sleep(0.01)
        yield inst, port
    finally:
        NetworkTableInstance.destroy(inst)


def request(port, method, target, body=None, **kwargs):
    conn = http.client.HTTPConnection("127.0.0.1", port, timeout=5)
    try:
        if body is not None:
            kwargs["body"] = json.dumps(body)
            kwargs["headers"] = {"Content-Type": "application/json"}
        conn.request(method, target, **kwargs)
        response = conn.getresponse()
        data = response.read()
        return response.status, json.loads(data) if data else None
    finally:
        conn.close()


def wait_for(predicate):
    deadline = time.monotonic() + 5
    while not predicate():
        assert time.monotonic() < deadline
        time.sleep(0.01)


def test_rest_subscribers(rest_server):
    server, port = rest_server
    client = NetworkTableInstance.create()
    local_sub = server.get_integer_topic("/rest/value").subscribe(0)
    remote_sub = client.get_integer_topic("/rest/value").subscribe(0)
    try:
        client.start_client("rest-test")
        client.set_server("127.0.0.1", port)
        wait_for(client.is_connected)
        path = "/nt/v1/topics/%2Frest%2Fvalue"
        status, data = request(port, "PUT", path, {"type": "int", "value": 42})
        assert status == 201
        assert data["properties"]["retained"] is True
        wait_for(lambda: local_sub.get() == 42 and remote_sub.get() == 42)
        assert request(port, "GET", path)[1]["value"] == 42
        assert request(port, "PATCH", path, {"properties": {"unit": "m"}})[0] == 204
        wait_for(lambda: remote_sub.get_topic().get_property("unit") == "m")
        assert request(port, "DELETE", path)[0] == 204
        wait_for(lambda: not remote_sub.get_topic().exists())
        assert not local_sub.get_topic().exists()
        assert request(port, "GET", path)[0] == 404
    finally:
        remote_sub.close()
        local_sub.close()
        NetworkTableInstance.destroy(client)


def test_rest_reads_and_updates_local_publisher(rest_server):
    server, port = rest_server
    topic = server.get_double_topic("/rest/local")
    pub = topic.publish()
    sub = topic.subscribe(0)
    try:
        pub.set(1.5)
        server.flush_local()
        path = "/nt/v1/topics/%2Frest%2Flocal"
        assert request(port, "GET", path)[1]["value"] == 1.5
        assert request(port, "PUT", path, {"value": 2.5})[0] == 200
        wait_for(lambda: sub.get() == 2.5)
        assert request(port, "DELETE", path)[0] == 409
        pub.set(3.5)
        server.flush_local()
        assert request(port, "GET", path)[1]["value"] == 3.5
    finally:
        sub.close()
        pub.close()


def test_rest_http_framing(rest_server):
    _, port = rest_server
    path = "/nt/v1/topics/%2Frest%2Fchunked"
    conn = http.client.HTTPConnection("127.0.0.1", port, timeout=5)
    try:
        conn.request(
            "PUT",
            path,
            body=iter([b'{"type":"int",', b'"value":42}']),
            headers={"Content-Type": "application/json; charset=utf-8"},
            encode_chunked=True,
        )
        response = conn.getresponse()
        assert response.status == 201
        assert json.loads(response.read())["value"] == 42
        # The same connection must reset its body and Content-Type for each request.
        conn.request("PUT", path, body='{"value":43}')
        response = conn.getresponse()
        assert response.status == 415
        response.read()
        conn.request("GET", path)
        response = conn.getresponse()
        assert response.status == 200
        assert json.loads(response.read())["value"] == 42
        conn.request("OPTIONS", path)
        response = conn.getresponse()
        assert response.status == 204
        assert "PUT" in response.getheader("Allow")
        assert (
            response.getheader("Access-Control-Allow-Headers") == "Content-Type, Accept"
        )
        response.read()
    finally:
        conn.close()


@pytest.mark.parametrize("chunked", [False, True])
def test_rest_body_limit(rest_server, chunked):
    _, port = rest_server
    conn = http.client.HTTPConnection("127.0.0.1", port, timeout=5)
    try:
        data = b" " * (2 * 1024 * 1024 + 1)
        conn.request(
            "PUT",
            "/nt/v1/topics/oversized",
            body=iter([data]) if chunked else data,
            headers={"Content-Type": "application/json"},
            encode_chunked=chunked,
        )
        response = conn.getresponse()
        assert response.status == 413
        assert response.getheader("Connection") == "close"
        assert "error" in json.loads(response.read())
    finally:
        conn.close()
    assert request(port, "GET", "/nt/v1/topics/oversized")[0] == 404


def test_rest_legacy_http_routes(rest_server):
    _, port = rest_server
    assert request(port, "GET", "/nt/persistent.json") == (200, [])
    conn = http.client.HTTPConnection("127.0.0.1", port, timeout=5)
    try:
        conn.request("GET", "/")
        response = conn.getresponse()
        assert response.status == 200
        assert b"/nt/v1/topics" in response.read()
    finally:
        conn.close()


def test_rest_persistence(rest_server, tmp_path):
    _, port = rest_server
    path = "/nt/v1/topics/%2Frest%2Fpersistent"
    persistent_file = tmp_path / "persistent.json"
    assert (
        request(
            port,
            "PUT",
            path,
            {"type": "int", "value": 42, "properties": {"persistent": True}},
        )[0]
        == 201
    )

    def saved():
        return json.loads(persistent_file.read_text())

    wait_for(lambda: len(saved()) == 1 and saved()[0]["value"] == 42)
    assert request(port, "PATCH", path, {"properties": {"unit": "m"}})[0] == 204
    wait_for(lambda: saved()[0]["properties"].get("unit") == "m")
    assert request(port, "GET", "/nt/persistent.json")[1][0]["value"] == 42
    assert request(port, "DELETE", path)[0] == 204
    wait_for(lambda: saved() == [])


def binary_request(
    port,
    method,
    path,
    body=None,
    content_type="application/msgpack",
    accept="application/msgpack",
):
    conn = http.client.HTTPConnection("127.0.0.1", port, timeout=5)
    try:
        conn.request(
            method,
            path,
            body=body,
            headers={"Content-Type": content_type, "Accept": accept},
        )
        response = conn.getresponse()
        return response.status, response.getheader("Content-Type"), response.read()
    finally:
        conn.close()


def test_rest_individual_resources(rest_server):
    server, port = rest_server
    path = "/nt/v1/topics/%2Frest%2Fpieces"
    sub = server.get_integer_topic("/rest/pieces").subscribe(0)
    try:
        assert request(port, "PUT", path, {"type": "int", "value": 42})[0] == 201
        assert request(port, "GET", path + "?type") == (200, "int")
        assert request(port, "GET", path + "?name") == (200, "/rest/pieces")
        assert request(port, "GET", path + "?timestamp")[1] > 0
        assert request(port, "GET", path + "?value") == (200, 42)
        assert request(port, "PUT", path + "?value", 43)[0] == 204
        assert sub.get() == 43
        assert binary_request(port, "PUT", path + "?value", b"\xd0\xfe")[0] == 204
        assert sub.get() == -2
        assert request(port, "GET", path + "?value") == (200, -2)
        assert binary_request(port, "GET", path + "?value") == (
            200,
            "application/msgpack",
            b"\xfe",
        )
        assert request(port, "PATCH", path + "?properties", {"unit": "m"})[0] == 204
        assert request(port, "GET", path + "?properties")[1]["unit"] == "m"
        assert request(port, "PUT", path + "?properties", {"retained": True})[0] == 204
        assert request(port, "GET", path + "?properties")[1] == {"retained": True}
    finally:
        sub.close()


@pytest.mark.parametrize("field", ["name", "type", "timestamp", "value", "properties"])
def test_rest_query_selectors_do_not_shadow_topics(rest_server, field):
    server, port = rest_server
    parent = "/nt/v1/topics/%2Frest"
    child = parent + "/" + field
    name = "/rest/" + field
    sub = server.get_integer_topic(name).subscribe(0)
    try:
        assert request(port, "PUT", parent, {"type": "int", "value": 42})[0] == 201
        assert request(port, "PUT", child, {"type": "int", "value": 1})[0] == 201
        assert sub.get() == 1
        assert request(port, "GET", child)[1]["name"] == name
        assert request(port, "GET", child + "?name") == (200, name)
        assert request(port, "GET", child + "?type") == (200, "int")
        assert request(port, "GET", child + "?timestamp")[1] > 0
        assert request(port, "GET", child + "?properties") == (200, {"retained": True})
        assert request(port, "GET", parent + "%2F" + field + "?value") == (200, 1)
        assert request(port, "GET", "/nt/v1/topics/" + name + "?value") == (200, 1)
        assert request(port, "PUT", child + "?value", 2)[0] == 204
        assert sub.get() == 2
        assert request(port, "GET", parent + "?value") == (200, 42)
        assert request(port, "DELETE", child + "?value&type")[0] == 400
        assert request(port, "DELETE", child)[0] == 204
        assert not sub.get_topic().exists()
        assert request(port, "GET", parent + "?value") == (200, 42)
    finally:
        sub.close()


def test_rest_messagepack_and_json_topics(rest_server):
    server, port = rest_server
    topic = server.get_raw_topic("/rest/msgpack")
    pub = topic.publish("msgpack")
    sub = topic.subscribe("msgpack", b"")
    path = "/nt/v1/topics/%2Frest%2Fmsgpack?value"
    # {"a": [1, true, null]} using standard MessagePack tags.
    packed = b"\x81\xa1a\x93\x01\xc3\xc0"
    logical = {"a": [1, True, None]}
    try:
        pub.set(packed)
        server.flush_local()
        assert request(port, "GET", path) == (200, logical)
        assert binary_request(port, "GET", path) == (200, "application/msgpack", packed)
        assert request(port, "PUT", path, {"b": 2})[0] == 204
        assert sub.get() == b"\x81\xa1b\x02"
        assert binary_request(port, "PUT", path, packed)[0] == 204
        assert sub.get() == packed
        assert binary_request(port, "PUT", path, packed + packed)[0] == 400
        assert sub.get() == packed
        binary = b"\xc4\x02\x00\xff"
        assert binary_request(port, "PUT", path, binary)[0] == 204
        assert request(port, "GET", path)[0] == 406
        assert binary_request(port, "GET", path)[2] == binary
    finally:
        sub.close()
        pub.close()
    json_path = "/nt/v1/topics/%2Frest%2Fjson"
    assert request(port, "PUT", json_path, {"type": "json", "value": "{}"})[0] == 201
    assert binary_request(port, "PUT", json_path + "?value", packed)[0] == 204
    assert request(port, "GET", json_path + "?value") == (200, logical)
    assert json.loads(request(port, "GET", json_path)[1]["value"]) == logical


def test_rest_messagepack_meta_topics(rest_server):
    server, port = rest_server
    # Creating a retained topic creates its $pub$ meta topic, an empty array.
    assert (
        request(port, "PUT", "/nt/v1/topics/test", {"type": "int", "value": 1})[0]
        == 201
    )
    path = "/nt/v1/topics/%24pub%24test?value"
    assert request(port, "GET", path) == (200, [])
    assert binary_request(port, "GET", path) == (200, "application/msgpack", b"\x90")
    assert binary_request(port, "PUT", path, b"\x90")[0] == 403
    # $serversub contains maps as well as arrays and reflects real subscriptions.
    sub = server.get_integer_topic("/rest/subscribed").subscribe(0)
    try:
        server.flush_local()
        status, subscriptions = request(port, "GET", "/nt/v1/topics/%24serversub?value")
        assert status == 200
        matching = [
            entry for entry in subscriptions if "/rest/subscribed" in entry["topics"]
        ]
        assert len(matching) == 1
        assert isinstance(matching[0]["uid"], int)
        assert isinstance(matching[0]["options"], dict)
    finally:
        sub.close()


def test_rest_negotiation_keep_alive(rest_server):
    _, port = rest_server
    path = "/nt/v1/topics/test"
    assert request(port, "PUT", path, {"type": "int", "value": 42})[0] == 201
    conn = http.client.HTTPConnection("127.0.0.1", port, timeout=5)
    try:
        conn.request(
            "GET",
            path + "?value",
            headers={"Accept": "application/json;q=0, application/msgpack"},
        )
        response = conn.getresponse()
        assert response.getheader("Content-Type") == "application/msgpack"
        assert response.getheader("Vary") == "Accept"
        assert response.read() == b"*"
        conn.request("GET", path + "?value")
        response = conn.getresponse()
        assert response.getheader("Content-Type") == "application/json"
        assert response.read() == b"42"
        conn.request(
            "PUT",
            path + "?value",
            body=iter([b"\xd1", b"\x01\x00"]),
            headers={"Content-Type": "application/msgpack"},
            encode_chunked=True,
        )
        response = conn.getresponse()
        assert response.status == 204
        response.read()
    finally:
        conn.close()
    assert request(port, "GET", path + "?value") == (200, 256)
    assert binary_request(port, "GET", path + "?value", accept="text/html")[0] == 406


@pytest.mark.parametrize("path", ["/nt/v1/persistent.json", "/nt/persistent.json"])
def test_rest_persistent_file_upload(rest_server, tmp_path, path):
    server, port = rest_server
    client = NetworkTableInstance.create()
    client.start_client("persistence-upload-test")
    client.set_server("127.0.0.1", port)
    local_sub = server.get_integer_topic("/rest/upload").subscribe(0)
    remote_sub = client.get_integer_topic("/rest/upload").subscribe(0)
    try:
        wait_for(client.is_connected)
        initial = [
            {
                "name": "/rest/upload",
                "type": "int",
                "value": 1,
                "properties": {"persistent": True, "old": "remove"},
            },
            {
                "name": "/rest/keep",
                "type": "int",
                "value": 2,
                "properties": {"persistent": True},
            },
        ]
        assert request(port, "PUT", path, initial)[0] == 204
        wait_for(lambda: local_sub.get() == 1 and remote_sub.get() == 1)
        assert (
            request(
                port,
                "PUT",
                "/nt/v1/topics/%2Frest%2Ftransient",
                {"type": "int", "value": 3},
            )[0]
            == 201
        )
        upload = [
            {
                "name": "/rest/upload",
                "type": "int",
                "value": 42,
                "properties": {"persistent": True, "unit": "m"},
            },
            {
                "name": "/rest/new",
                "type": "raw",
                "value": "AP8=",
                "properties": {"persistent": True},
            },
        ]
        assert request(port, "PUT", path, upload)[0] == 204
        wait_for(lambda: local_sub.get() == 42 and remote_sub.get() == 42)
        wait_for(lambda: remote_sub.get_topic().get_property("unit") == "m")
        expected = {entry["name"]: entry for entry in [*upload, initial[1]]}
        status, downloaded = request(port, "GET", path)
        assert status == 200
        assert {entry["name"]: entry for entry in downloaded} == expected
        assert all("timestamp" not in entry for entry in downloaded)
        assert (
            request(port, "GET", "/nt/v1/topics/%2Frest%2Ftransient")[1]["value"] == 3
        )
        assert request(port, "PUT", path, [upload[0], {"invalid": True}])[0] == 400
        assert request(port, "PUT", path, [dict(upload[0], type="double")])[0] == 409
        assert request(port, "PUT", path, [upload[0], upload[0]])[0] == 400
        assert {
            entry["name"]: entry for entry in request(port, "GET", path)[1]
        } == expected
        assert request(port, "PUT", path, [])[0] == 204

        persistent_file = tmp_path / "persistent.json"

        def saved():
            try:
                return {
                    entry["name"]: entry
                    for entry in json.loads(persistent_file.read_text())
                }
            except (FileNotFoundError, json.JSONDecodeError):
                return None

        wait_for(lambda: saved() == expected)
        # A new server can restore the downloaded file without conversion.
        server.stop_server()
        wait_for(lambda: not client.is_connected())
        persistent_file.write_text(json.dumps(downloaded))
        server.start_server(str(persistent_file), "127.0.0.1", "", port)
        wait_for(client.is_connected)
        wait_for(lambda: remote_sub.get() == 42)
        assert {
            entry["name"]: entry for entry in request(port, "GET", path)[1]
        } == expected
    finally:
        remote_sub.close()
        local_sub.close()
        NetworkTableInstance.destroy(client)
