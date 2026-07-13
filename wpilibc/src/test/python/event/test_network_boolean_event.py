import pytest

from wpilib import EventLoop, NetworkBooleanEvent


@pytest.fixture
def nt_inst(wpilib_state):
    import ntcore

    inst = ntcore.NetworkTableInstance.get_default()
    inst.start_local()
    try:
        yield inst
    finally:
        inst.stop_local()
        inst._reset()


def test_set(nt_inst):
    loop = EventLoop()
    counter = 0

    def on_high() -> None:
        nonlocal counter
        counter += 1

    pub = nt_inst.get_table("TestTable").get_boolean_topic("Test").publish()

    NetworkBooleanEvent(loop, nt_inst, "TestTable", "Test").if_high(on_high)

    pub.set(False)
    loop.poll()
    assert counter == 0

    pub.set(True)
    loop.poll()
    assert counter == 1

    pub.set(False)
    loop.poll()
    assert counter == 1
