import pytest

from wpilib import EventLoop


def test_bind_unbind():
    loop = EventLoop()

    pollCount = 0

    def count_poll():
        nonlocal pollCount
        pollCount += 1

    task1 = loop.bind(count_poll)
    task2 = loop.bind(count_poll)
    loop.poll()

    assert pollCount == 2

    loop.unbind(task1)
    loop.poll()

    assert pollCount == 3

    loop.unbind(task2)

    loop.poll()

    assert pollCount == 3


def test_concurrent_modification():
    loop = EventLoop()

    def bind_during_poll():
        with pytest.raises(RuntimeError):
            loop.bind(lambda: None)

    loop.bind(bind_during_poll)
    loop.poll()

    loop.clear()

    taskId = None

    def unbind_during_poll():
        with pytest.raises(RuntimeError):
            loop.unbind(taskId)

    taskId = loop.bind(unbind_during_poll)
    loop.poll()

    loop.clear()

    def clear_during_poll():
        with pytest.raises(RuntimeError):
            loop.clear()

    loop.bind(clear_during_poll)
    loop.poll()
