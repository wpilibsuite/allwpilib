import threading
import time

from commands2.button.commandgenerichid import (
    CommandGenericHID,
    _reset_command_generic_hid_data,
)


def test_get_command_generic_hid_is_thread_safe(monkeypatch):
    monkeypatch.setattr(CommandGenericHID, "_hids", {})

    init_count = 0
    init_count_lock = threading.Lock()

    def fake_init(self, port):
        nonlocal init_count
        with init_count_lock:
            init_count += 1
        time.sleep(0.02)
        self._hid = object()

    monkeypatch.setattr(CommandGenericHID, "__init__", fake_init)

    thread_count = 16
    start = threading.Barrier(thread_count)
    results = [None] * thread_count
    exceptions = []

    def worker(index):
        try:
            start.wait()
            results[index] = CommandGenericHID.get_command_generic_hid(0)
        except Exception as exc:
            exceptions.append(exc)

    threads = [threading.Thread(target=worker, args=(i,)) for i in range(thread_count)]
    for thread in threads:
        thread.start()
    for thread in threads:
        thread.join()

    assert not exceptions
    assert init_count == 1
    assert len({id(result) for result in results}) == 1


def test_reset_command_generic_hid_data(monkeypatch):
    first = object()
    monkeypatch.setattr(CommandGenericHID, "_hids", {0: first})

    _reset_command_generic_hid_data()

    assert CommandGenericHID._hids == {}
