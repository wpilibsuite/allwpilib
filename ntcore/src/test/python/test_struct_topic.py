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
    topic = nt.get_struct_topic("mystruct", MyStruct)
    pub = topic.publish()
    sub = topic.subscribe(MyStruct(1, True, 2.0))

    assert topic.get_type_string() == "struct:MyStruct"

    default = MyStruct(6, False, 7.0)
    pub.set_default(default)
    val = sub.get()
    assert val == default

    # val2 = MyStruct(0, False, 0)
    # sub.getInto(val2)
    # assert val2 == default

    vals = sub.read_queue()
    assert len(vals) == 1
    assert vals[0].value == default


def test_mystruct_array(nt: ntcore.NetworkTableInstance):
    topic = nt.get_struct_array_topic("mystruct", MyStruct)
    pub = topic.publish()
    sub = topic.subscribe([])

    assert topic.get_type_string() == "struct:MyStruct[]"

    default = MyStruct(8, True, 9.0)
    pub.set_default([default])
    val = sub.get()
    assert val == [default]

    val2 = MyStruct(0, False, 0)
    pub.set([val2])
    atomic_val = sub.get_atomic()
    assert atomic_val.value == [val2]

    vals = sub.read_queue()
    assert len(vals) == 1
    assert vals[0].value == [val2]
