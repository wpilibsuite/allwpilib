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
        wpistruct.get_schema(str)


def test_for_each_nested():
    l = []

    def _fn(*args):
        l.append(args)

    wpistruct.for_each_nested(module.ThingA, _fn)
    assert l == [("struct:ThingA", "uint8 value")]


def test_get_type_string():
    assert wpistruct.get_type_name(module.ThingA) == "ThingA"


def test_get_schema():
    assert wpistruct.get_schema(module.ThingA) == "uint8 value"


def test_get_size():
    assert wpistruct.get_size(module.ThingA) == 1


def test_pack():
    assert wpistruct.pack(module.ThingA(1)) == b"\x01"


def test_pack_array():
    assert wpistruct.pack_array([module.ThingA(1), module.ThingA(2)]) == b"\x01\x02"


def test_pack_into():
    buf = bytearray(1)
    wpistruct.pack_into(module.ThingA(1), buf)
    assert buf == b"\x01"


def test_pack_into_err():
    buf = bytearray(2)
    with pytest.raises(ValueError, match=re.escape("buffer must be 1 bytes")):
        wpistruct.pack_into(module.ThingA(1), buf)


def test_unpack():
    assert wpistruct.unpack(module.ThingA, b"\x01") == module.ThingA(1)


def test_unpack_array():
    assert wpistruct.unpack_array(module.ThingA, b"\x01\x02") == [
        module.ThingA(1),
        module.ThingA(2),
    ]


# def test_unpack_into():
#     r1 = module.ThingA(1)
#     r2 = module.ThingA(2)
#     assert r1 != r2
#     wpistruct.unpack_into(b"\x01", r2)
#     assert r1 == r2


#
# Nested struct
#


def test_nested_for_each_nested():
    l = []

    def _fn(*args):
        l.append(args)

    wpistruct.for_each_nested(module.Outer, _fn)
    assert l == [
        ("struct:ThingA", "uint8 value"),
        ("struct:Outer", "ThingA inner; int32 c"),
    ]


def test_nested_get_type_string():
    assert wpistruct.get_type_name(module.ThingA) == "ThingA"


def test_nested_get_schema():
    assert wpistruct.get_schema(module.Outer) == "ThingA inner; int32 c"


def test_nested_get_size():
    assert wpistruct.get_size(module.Outer) == 5


def test_nested_pack():
    v = module.Outer(module.ThingA(2), 4)
    assert wpistruct.pack(v) == b"\x02\x04\x00\x00\x00"


def test_nested_pack_into():
    v = module.Outer(module.ThingA(3), 5)
    buf = bytearray(5)
    wpistruct.pack_into(v, buf)
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

    wpistruct.for_each_nested(MyStruct, _fn)
    assert l == [("struct:mystruct", "int32 x; bool y; float z")]


def test_user_get_type_string():
    assert wpistruct.get_type_name(MyStruct) == "mystruct"


def test_user_get_schema():
    assert wpistruct.get_schema(MyStruct) == "int32 x; bool y; float z"


def test_user_get_size():
    assert wpistruct.get_size(MyStruct) == 9


def test_user_pack():
    v = MyStruct(2, True, 3.5)
    assert wpistruct.pack(v) == b"\x02\x00\x00\x00\x01\x00\x00\x60\x40"


def test_user_pack_into():
    v = MyStruct(2, True, 3.5)
    buf = bytearray(9)
    wpistruct.pack_into(v, buf)
    assert buf == b"\x02\x00\x00\x00\x01\x00\x00\x60\x40"


def test_user_unpack():
    v = MyStruct(2, True, 3.5)
    assert wpistruct.unpack(MyStruct, b"\x02\x00\x00\x00\x01\x00\x00\x60\x40") == v


@wpistruct.make_wpistruct(name="VectorStruct")
@dataclasses.dataclass
class VectorStruct:
    data: tuple[wpistruct.double, wpistruct.double, wpistruct.double]


def test_user_tuple_array_get_schema():
    assert wpistruct.get_schema(VectorStruct) == "double data[3]"


def test_user_tuple_array_get_size():
    assert wpistruct.get_size(VectorStruct) == 24


def test_user_tuple_array_pack():
    assert wpistruct.pack(VectorStruct((1.0, 2.0, 3.0))) == (
        b"\x00\x00\x00\x00\x00\x00\xf0?"
        b"\x00\x00\x00\x00\x00\x00\x00@"
        b"\x00\x00\x00\x00\x00\x00\x08@"
    )


def test_user_tuple_array_unpack():
    assert wpistruct.unpack(
        VectorStruct,
        b"\x00\x00\x00\x00\x00\x00\xf0?"
        b"\x00\x00\x00\x00\x00\x00\x00@"
        b"\x00\x00\x00\x00\x00\x00\x08@",
    ) == VectorStruct((1.0, 2.0, 3.0))


def test_user_tuple_array_rejects_mixed_types():
    with pytest.raises(
        TypeError,
        match=re.escape(
            "MixedTuple.value has unsupported tuple type hint: "
            "tuple fields must be fixed-length and homogeneous"
        ),
    ):

        @wpistruct.make_wpistruct
        @dataclasses.dataclass
        class MixedTuple:
            value: tuple[int, float]


def test_user_rejects_unsupported_type_with_tuple_in_supported_list():
    with pytest.raises(
        TypeError,
        match=re.escape(
            "BadField.value is not a wpistruct or does not have a supported type hint "
            "(supported: bool, int8, uint8, int16, uint16, int, int32, uint32, "
            "int64, uint64, float, double, or fixed-length homogeneous tuple of "
            "a supported type)"
        ),
    ):

        @wpistruct.make_wpistruct
        @dataclasses.dataclass
        class BadField:
            value: str


# def test_user_unpack_into():
#     v1 = MyStruct(2, True, 3.5)
#     v2 = MyStruct(3, True, 4.5)
#     assert v1 != v2
#     wpistruct.unpack_into(b"\x02\x00\x00\x00\x01\x00\x00\x60\x40", v2)
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

    wpistruct.for_each_nested(Outer, _fn)
    assert l == [
        ("struct:mystruct", "int32 x; bool y; float z"),
        ("struct:Outer", "int32 x; mystruct inner"),
    ]


def test_user_nested_get_type_string():
    assert wpistruct.get_type_name(Outer) == "Outer"


def test_user_nested_get_schema():
    assert wpistruct.get_schema(Outer) == "int32 x; mystruct inner"


def test_user_nested_get_size():
    assert wpistruct.get_size(Outer) == 4 + 9


def test_user_nested_pack():
    v = Outer(2, MyStruct(3, True, 4.0))
    assert wpistruct.pack(v) == b"\x02\x00\x00\x00\x03\x00\x00\x00\x01\x00\x00\x80\x40"


def test_user_nested_pack_into():
    v = Outer(2, MyStruct(3, True, 4.0))
    buf = bytearray(4 + 9)
    wpistruct.pack_into(v, buf)
    assert buf == b"\x02\x00\x00\x00\x03\x00\x00\x00\x01\x00\x00\x80\x40"


def test_user_nested_unpack():
    assert wpistruct.unpack(
        Outer, b"\x02\x00\x00\x00\x03\x00\x00\x00\x01\x00\x00\x80\x40"
    ) == Outer(2, MyStruct(3, True, 4.0))
