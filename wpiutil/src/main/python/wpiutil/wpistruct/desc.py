import typing

if typing.TYPE_CHECKING:
    from typing_extensions import Buffer
else:
    # Avoiding typing_extensions runtime dependency
    Buffer = bytearray


class StructDescriptor(typing.NamedTuple):
    """
    To define a type in python that can use the wpilib serialization, the type must
    have an attribute `WPIStruct` that contains this class (but C++ classes
    do not have this).

    It is not intended that you should create this class directly, something
    else should generate it for you.

    See :func:`wpiutil.wpistruct.make_wpistruct` for a easy to use mechanism
    for defining custom structs using a dataclass.
    """

    #: The type name
    typename: str

    #: The struct schema
    schema: str

    #: Size in bytes of binary representation of this struct
    size: int

    #: A function that converts the type to bytes
    pack: typing.Callable[[typing.Any], bytes]

    #: A function that converts the type to bytes
    packInto: typing.Callable[[typing.Any, Buffer], None]

    #: A function that converts bytes to an instance
    unpack: typing.Callable[[Buffer], typing.Any]

    #: A function that updates the given instance using the deserialized bytes
    #: .. not supported
    # unpackInto: typing.Callable[[typing.Any, Buffer], None]

    #: If this contains nested structs, calls wpiutil.wpistruct.forEachNested for each
    forEachNested: typing.Optional[
        typing.Callable[[typing.Callable[[str, str], None]], None]
    ]
