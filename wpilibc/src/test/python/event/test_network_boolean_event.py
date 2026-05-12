import pytest

from wpilib import EventLoop, NetworkBooleanEvent


@pytest.fixture
def nt_inst(wpilib_state):
    import ntcore

    inst = ntcore.NetworkTableInstance.getDefault()
    inst.startLocal()
    try:
        yield inst
    finally:
        inst.stopLocal()
        inst._reset()


def test_set(nt_inst):
    loop = EventLoop()
    counter = [0]

    pub = nt_inst.getTable("TestTable").getBooleanTopic("Test").publish()

    NetworkBooleanEvent(loop, nt_inst, "TestTable", "Test").ifHigh(
        lambda: counter.__setitem__(0, counter[0] + 1)
    )

    pub.set(False)
    loop.poll()
    assert counter[0] == 0

    pub.set(True)
    loop.poll()
    assert counter[0] == 1

    pub.set(False)
    loop.poll()
    assert counter[0] == 1
