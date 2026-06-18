import dataclasses
import re

import pytest

from wpiutil import wpistruct
from wpiutil_test import module

#
# Static serialization
#


# ensure that a type that doesn't work has a sane error message
def test_invalid_type():
    with pytest.raises(
        TypeError,
        match=re.escape("str is not struct serializable (does not have WPIStruct)"),
    ):
        wpistruct.getSchema(str)


def test_for_each_nested():
    l = []

    def _fn(*args):
        l.append(args)

    wpistruct.forEachNested(module.ThingA, _fn)
    assert l == [("struct:ThingA", "uint8 value")]


def test_get_type_string():
    assert wpistruct.getTypeName(module.ThingA) == "ThingA"


def test_get_schema():
    assert wpistruct.getSchema(module.ThingA) == "uint8 value"


def test_get_size():
    assert wpistruct.getSize(module.ThingA) == 1


def test_pack():
    assert wpistruct.pack(module.ThingA(1)) == b"\x01"


def test_pack_array():
    assert wpistruct.packArray([module.ThingA(1), module.ThingA(2)]) == b"\x01\x02"


def test_pack_into():
    buf = bytearray(1)
    wpistruct.packInto(module.ThingA(1), buf)
    assert buf == b"\x01"


def test_pack_into_err():
    buf = bytearray(2)
    with pytest.raises(ValueError, match=re.escape("buffer must be 1 bytes")):
        wpistruct.packInto(module.ThingA(1), buf)


def test_unpack():
    assert wpistruct.unpack(module.ThingA, b"\x01") == module.ThingA(1)


def test_unpack_array():
    assert wpistruct.unpackArray(module.ThingA, b"\x01\x02") == [
        module.ThingA(1),
        module.ThingA(2),
    ]


# def test_unpack_into():
#     r1 = module.ThingA(1)
#     r2 = module.ThingA(2)
#     assert r1 != r2
#     wpistruct.unpackInto(b"\x01", r2)
#     assert r1 == r2


#
# Nested struct
#


def test_nested_for_each_nested():
    l = []

    def _fn(*args):
        l.append(args)

    wpistruct.forEachNested(module.Outer, _fn)
    assert l == [
        ("struct:ThingA", "uint8 value"),
        ("struct:Outer", "ThingA inner; int32 c"),
    ]


def test_nested_get_type_string():
    assert wpistruct.getTypeName(module.ThingA) == "ThingA"


def test_nested_get_schema():
    assert wpistruct.getSchema(module.Outer) == "ThingA inner; int32 c"


def test_nested_get_size():
    assert wpistruct.getSize(module.Outer) == 5


def test_nested_pack():
    v = module.Outer(module.ThingA(2), 4)
    assert wpistruct.pack(v) == b"\x02\x04\x00\x00\x00"


def test_nested_pack_into():
    v = module.Outer(module.ThingA(3), 5)
    buf = bytearray(5)
    wpistruct.packInto(v, buf)
    assert buf == b"\x03\x05\x00\x00\x00"


def test_nested_unpack():
    assert wpistruct.unpack(module.ThingA, b"\x01") == module.ThingA(1)


#
# User defined serialization
#


@wpistruct.make_wpistruct(name="mystruct")
@dataclasses.dataclass
class MyStruct:
    x: int
    y: bool
    z: float


def test_user_for_each_nested():
    l = []

    def _fn(*args):
        l.append(args)

    wpistruct.forEachNested(MyStruct, _fn)
    assert l == [("struct:mystruct", "int32 x; bool y; float z")]


def test_user_get_type_string():
    assert wpistruct.getTypeName(MyStruct) == "mystruct"


def test_user_get_schema():
    assert wpistruct.getSchema(MyStruct) == "int32 x; bool y; float z"


def test_user_get_size():
    assert wpistruct.getSize(MyStruct) == 9


def test_user_pack():
    v = MyStruct(2, True, 3.5)
    assert wpistruct.pack(v) == b"\x02\x00\x00\x00\x01\x00\x00\x60\x40"


def test_user_pack_into():
    v = MyStruct(2, True, 3.5)
    buf = bytearray(9)
    wpistruct.packInto(v, buf)
    assert buf == b"\x02\x00\x00\x00\x01\x00\x00\x60\x40"


def test_user_unpack():
    v = MyStruct(2, True, 3.5)
    assert wpistruct.unpack(MyStruct, b"\x02\x00\x00\x00\x01\x00\x00\x60\x40") == v


# def test_user_unpack_into():
#     v1 = MyStruct(2, True, 3.5)
#     v2 = MyStruct(3, True, 4.5)
#     assert v1 != v2
#     wpistruct.unpackInto(b"\x02\x00\x00\x00\x01\x00\x00\x60\x40", v2)
#     assert v1 == v2


#
# User defined serialization (nested)
#


@wpistruct.make_wpistruct
@dataclasses.dataclass
class Outer:
    x: int
    inner: MyStruct


def test_user_nested_for_each_nested():
    l = []

    def _fn(*args):
        l.append(args)

    wpistruct.forEachNested(Outer, _fn)
    assert l == [
        ("struct:mystruct", "int32 x; bool y; float z"),
        ("struct:Outer", "int32 x; mystruct inner"),
    ]


def test_user_nested_get_type_string():
    assert wpistruct.getTypeName(Outer) == "Outer"


def test_user_nested_get_schema():
    assert wpistruct.getSchema(Outer) == "int32 x; mystruct inner"


def test_user_nested_get_size():
    assert wpistruct.getSize(Outer) == 4 + 9


def test_user_nested_pack():
    v = Outer(2, MyStruct(3, True, 4.0))
    assert wpistruct.pack(v) == b"\x02\x00\x00\x00\x03\x00\x00\x00\x01\x00\x00\x80\x40"


def test_user_nested_pack_into():
    v = Outer(2, MyStruct(3, True, 4.0))
    buf = bytearray(4 + 9)
    wpistruct.packInto(v, buf)
    assert buf == b"\x02\x00\x00\x00\x03\x00\x00\x00\x01\x00\x00\x80\x40"


def test_user_nested_unpack():
    assert wpistruct.unpack(
        Outer, b"\x02\x00\x00\x00\x03\x00\x00\x00\x01\x00\x00\x80\x40"
    ) == Outer(2, MyStruct(3, True, 4.0))
