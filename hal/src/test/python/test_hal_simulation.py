import hal
import hal.simulation

import typing


def test_value_changed_callback():

    recv: typing.Optional[typing.Tuple[bool, str, int]] = None

    def created_cb(
        name: str, handle: int, direction: hal.SimValueDirection, value: hal.Value
    ):
        nonlocal recv
        recv = (True, name, value.value)

    def cb(name: str, handle: int, direction: hal.SimValueDirection, value: hal.Value):
        nonlocal recv
        recv = (False, name, value.value)

    dev = hal.SimDevice("simd")

    # Must keep the returned value alive or the callback will be unregistered
    devunused = hal.simulation.registerSimValueCreatedCallback(dev, created_cb, True)
    assert recv is None

    val = dev.createInt("answer", 0, 42)

    assert recv == (True, "answer", 42)
    recv = None

    # Must keep the returned value alive or the callback will be unregistered
    unused = hal.simulation.registerSimValueChangedCallback(val, cb, True)

    assert recv == (False, "answer", 42)
    recv = None

    val.set(84)

    assert recv == (False, "answer", 84)
    recv = None
