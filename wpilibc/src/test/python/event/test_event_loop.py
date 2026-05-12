import pytest

from wpilib import EventLoop


def test_concurrent_modification():
    loop = EventLoop()

    def bind_during_poll():
        with pytest.raises(RuntimeError):
            loop.bind(lambda: None)

    loop.bind(bind_during_poll)
    loop.poll()

    loop.clear()

    def clear_during_poll():
        with pytest.raises(RuntimeError):
            loop.clear()

    loop.bind(clear_during_poll)
    loop.poll()
