import dataclasses
import inspect
import struct
import typing


from .desc import StructDescriptor
from .._wpiutil import wpistruct

#
# Use these types to specify explicitly sized integers, but you can
# also use int/bool/float
#

# fmt: off
    
if typing.TYPE_CHECKING:
    int8 = int
    uint8 = int
    int16 = int
    uint16 = int
    int32 = int
    uint32 = int
    int64 = int
    uint64 = int
    double = float
else:
    class int8(int): pass
    class uint8(int): pass
    class int16(int): pass
    class uint16(int): pass
    class int32(int): pass
    class uint32(int): pass
    class int64(int): pass
    class uint64(int): pass

    class double(float): pass

# fmt: on


def make_wpistruct(cls=None, /, *, name: typing.Optional[str] = None):
    """
    This decorator allows you to easily define a custom type that can be
    used with wpilib's custom serialization protocol (for use in datalog
    and networktables). Just create a normal python dataclass, and apply
    this decorator to the class.

    For example, here's how you define a dataclass that contains an integer,
    a boolean, and a double::

        @wpiutil.wpistruct.make_wpistruct(name="mystruct")
        @dataclasses.dataclass
        class MyStruct:
            x: wpiutil.wpistruct.int32
            y: bool
            z: wpiutil.struct.double

    The types defined in the dataclass can be another WPIStruct compatible class
    (either builtin or user defined); one of int, bool, or float; or you can
    use one of the ``wpiutil.wpistruct.[u]int*`` values for explicitly sized
    integer types.
    """

    def wrap(cls):
        return _process_class(cls, name)

    if cls is None:
        return wrap

    return wrap(cls)


#
# Internals
#

_type_to_fmt = {
    bool: ("?", "bool"),
    int8: ("b", "int8"),
    uint8: ("B", "uint8"),
    int16: ("h", "int16"),
    uint16: ("H", "uint16"),
    int: ("i", "int32"),
    int32: ("i", "int32"),
    uint32: ("I", "uint32"),
    int64: ("q", "int64"),
    uint64: ("Q", "uint64"),
    float: ("f", "float"),
    double: ("d", "double"),
}


def _process_class(cls, struct_name: typing.Optional[str]):
    resolved_hints = typing.get_type_hints(cls)
    field_names = [field.name for field in dataclasses.fields(cls)]
    resolved_field_types = {name: resolved_hints[name] for name in field_names}

    name_parts = []
    name_parts.append(getattr(cls, "__module__", None))
    name_parts.append(getattr(cls, "__qualname__", cls.__name__))
    cls_name = ".".join([n for n in name_parts if n])

    if struct_name is None:
        struct_name = cls.__name__
        err_name = cls_name
    else:
        err_name = f"{struct_name} ({cls_name})"

    fmts = []
    schema = []
    cvvals = []
    vvals = []
    packs = []
    unpacks = []
    # unpackIntos = []
    forEachNested = []

    ctx: typing.Dict[str, typing.Any] = {"cls": cls}

    for name, ftype in resolved_field_types.items():
        if ftype in _type_to_fmt:
            fmt, stype = _type_to_fmt[ftype]

            fmts.append(fmt)
            schema.append(f"{stype} {name}")
            cvvals.append(f"arg_{name}")
            vvals.append(f"v.{name}")

        elif hasattr(ftype, "WPIStruct"):
            # nested struct
            argn = f"arg_{name}"
            typn = f"type_{name}"

            ctx[typn] = ftype
            ts = wpistruct.getTypeName(ftype)
            schema.append(f"{ts} {name}")
            sz = wpistruct.getSize(ftype)
            fmts.append(f"{sz}s")
            vvals.append(argn)
            cvvals.append(argn)
            packs.append(f"{argn} = wpistruct.pack(v.{name})")
            unpacks.append(f"{argn} = wpistruct.unpack({typn}, {argn})")
            # unpackIntos.append(f"wpistruct.unpackInto(v.{name}, {argn})")
            forEachNested.append(f"wpistruct.forEachNested({typn}, fn)")

        else:
            supported_names = ", ".join(t.__name__ for t in _type_to_fmt.keys())
            raise TypeError(
                f"{cls_name}.{name} is not a wpistruct or does not have a supported type hint "
                f"(supported: {supported_names})"
            ) from None

    s = struct.Struct(f"<{''.join(fmts)}")
    cvals = ", ".join(cvvals)
    vals = ", ".join(vvals)

    padding = "\n" + " " * 16
    pack_stmts = padding.join(packs)
    unpack_stmts = padding.join(unpacks)
    # unpackInto_stmts = padding.join(unpackIntos)

    if not forEachNested:
        forEachNested_stmt = "_forEachNested = None"
    else:
        forEachNested_stmt = f"def _forEachNested(fn):"
        forEachNested_stmt += "\n" + " " * 12
        forEachNested_stmt += f"try:{padding}"
        forEachNested_stmt += padding.join(forEachNested)
        forEachNested_stmt += "\n" + " " * 12
        forEachNested_stmt += f"except Exception as e:"
        forEachNested_stmt += (
            f"{padding}raise ValueError(f'{err_name}: error in forEachNested') from e"
        )

    ctx["_s"] = s

    # Construct the serialization functions using the same hack NamedTuple uses
    fnsrc = inspect.cleandoc(
        f"""
        from wpiutil import wpistruct

        def _pack(v):
            try:
                {pack_stmts}
                return _s.pack({vals})
            except Exception as e:
                raise ValueError(f"{err_name}: error packing data") from e
                            
        def _packInto(v, b):
            try:
                {pack_stmts}
                return _s.pack_into(b, 0, {vals})
            except Exception as e:
                raise ValueError(f"{err_name}: error packing data") from e

        def _unpack(b):
            try:
                {cvals} = _s.unpack(b)
                {unpack_stmts}
                return cls({cvals})
            except Exception as e:
                raise ValueError(f"{err_name}: error unpacking data") from e
        
        #def _unpackInto(v, b):
        #    try:
        #        {vals} = _s.unpack(b)
        #        {{unpackInto_stmts}}
        #    except Exception as e:
        #        raise ValueError(f"{err_name}: error unpacking data") from e

        {forEachNested_stmt}
    """
    )

    exec(fnsrc, ctx, ctx)

    cls.WPIStruct = StructDescriptor(
        typename=struct_name,
        schema="; ".join(schema),
        size=s.size,
        pack=ctx["_pack"],
        packInto=ctx["_packInto"],
        unpack=ctx["_unpack"],
        # unpackInto=ctx["_unpackInto"],
        forEachNested=ctx["_forEachNested"],
    )

    return cls
