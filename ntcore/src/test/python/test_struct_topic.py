import dataclasses

import ntcore
from wpiutil import wpistruct


@wpistruct.make_wpistruct
@dataclasses.dataclass
class MyStruct:
    x: int
    y: bool
    z: wpistruct.double


def test_mystruct(nt: ntcore.NetworkTableInstance):
    topic = nt.getStructTopic("mystruct", MyStruct)
    pub = topic.publish()
    sub = topic.subscribe(MyStruct(1, True, 2.0))

    assert topic.getTypeString() == "struct:MyStruct"

    default = MyStruct(6, False, 7.0)
    pub.setDefault(default)
    val = sub.get()
    assert val == default

    # val2 = MyStruct(0, False, 0)
    # sub.getInto(val2)
    # assert val2 == default

    vals = sub.readQueue()
    assert len(vals) == 1
    assert vals[0].value == default


def test_mystruct_array(nt: ntcore.NetworkTableInstance):
    topic = nt.getStructArrayTopic("mystruct", MyStruct)
    pub = topic.publish()
    sub = topic.subscribe([])

    assert topic.getTypeString() == "struct:MyStruct[]"

    default = MyStruct(8, True, 9.0)
    pub.setDefault([default])
    val = sub.get()
    assert val == [default]

    val2 = MyStruct(0, False, 0)
    pub.set([val2])
    atomicVal = sub.getAtomic()
    assert atomicVal.value == [val2]

    vals = sub.readQueue()
    assert len(vals) == 1
    assert vals[0].value == [val2]
