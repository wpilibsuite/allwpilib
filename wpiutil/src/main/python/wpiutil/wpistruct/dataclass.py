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
    (either builtin or user defined); one of int, bool, or float; a fixed-length
    homogeneous tuple of those supported types; or you can use one of the
    ``wpiutil.wpistruct.[u]int*`` values for explicitly sized integer types.
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


def _get_supported_type_names():
    supported_names = ", ".join(t.__name__ for t in _type_to_fmt.keys())
    return f"{supported_names}, or fixed-length homogeneous tuple of a supported type"


def _get_fixed_tuple_array_info(cls_name: str, field_name: str, ftype: type):
    origin = typing.get_origin(ftype)
    if origin is not tuple:
        return None

    args = typing.get_args(ftype)
    if not args or args[-1] is Ellipsis:
        raise TypeError(
            f"{cls_name}.{field_name} has unsupported tuple type hint: "
            "tuple fields must be fixed-length and homogeneous"
        ) from None

    element_type = args[0]
    if not all(arg == element_type for arg in args):
        raise TypeError(
            f"{cls_name}.{field_name} has unsupported tuple type hint: "
            "tuple fields must be fixed-length and homogeneous"
        ) from None

    return element_type, len(args)


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
    unpackvals = []
    cvvals = []
    vvals = []
    packs = []
    unpacks = []
    # unpack_intos = []
    for_each_nested = []

    ctx: typing.Dict[str, typing.Any] = {"cls": cls}

    for field_idx, (name, ftype) in enumerate(resolved_field_types.items()):
        if ftype in _type_to_fmt:
            fmt, stype = _type_to_fmt[ftype]

            fmts.append(fmt)
            schema.append(f"{stype} {name}")
            unpackvals.append(f"arg_{name}")
            cvvals.append(f"arg_{name}")
            vvals.append(f"v.{name}")

        elif array_info := _get_fixed_tuple_array_info(cls_name, name, ftype):
            element_type, array_len = array_info
            argn = f"arg_{name}"
            unpack_args = [f"arg{field_idx}_{i}" for i in range(array_len)]

            if element_type in _type_to_fmt:
                fmt, stype = _type_to_fmt[element_type]

                fmts.append(f"{array_len}{fmt}")
                schema.append(f"{stype} {name}[{array_len}]")
                unpackvals.extend(unpack_args)
                cvvals.append(argn)
                vvals.append(f"*v.{name}")
                unpacks.append(f"{argn} = ({', '.join(unpack_args)},)")

            elif hasattr(element_type, "WPIStruct"):
                typn = f"type_{name}"

                ctx[typn] = element_type
                ts = wpistruct.get_type_name(element_type)
                schema.append(f"{ts} {name}[{array_len}]")
                sz = wpistruct.get_size(element_type)
                fmts.extend(f"{sz}s" for _ in range(array_len))
                unpackvals.extend(unpack_args)
                vvals.append(f"*{argn}")
                cvvals.append(argn)
                packs.append(f"{argn} = tuple(wpistruct.pack(i) for i in v.{name})")
                unpack_exprs = [f"wpistruct.unpack({typn}, {a})" for a in unpack_args]
                unpacks.append(f"{argn} = ({', '.join(unpack_exprs)},)")
                # unpack_intos.append(f"wpistruct.unpack_into(v.{name}, {argn})")
                for_each_nested.append(f"wpistruct.for_each_nested({typn}, fn)")

            else:
                raise TypeError(
                    f"{cls_name}.{name} is not a wpistruct or does not have a supported type hint "
                    f"(supported: {_get_supported_type_names()})"
                ) from None

        elif hasattr(ftype, "WPIStruct"):
            # nested struct
            argn = f"arg_{name}"
            typn = f"type_{name}"

            ctx[typn] = ftype
            ts = wpistruct.get_type_name(ftype)
            schema.append(f"{ts} {name}")
            sz = wpistruct.get_size(ftype)
            fmts.append(f"{sz}s")
            vvals.append(argn)
            unpackvals.append(argn)
            cvvals.append(argn)
            packs.append(f"{argn} = wpistruct.pack(v.{name})")
            unpacks.append(f"{argn} = wpistruct.unpack({typn}, {argn})")
            # unpack_intos.append(f"wpistruct.unpack_into(v.{name}, {argn})")
            for_each_nested.append(f"wpistruct.for_each_nested({typn}, fn)")

        else:
            raise TypeError(
                f"{cls_name}.{name} is not a wpistruct or does not have a supported type hint "
                f"(supported: {_get_supported_type_names()})"
            ) from None

    s = struct.Struct(f"<{''.join(fmts)}")
    uvals = ", ".join(unpackvals)
    if len(unpackvals) == 1:
        uvals += ","
    cvals = ", ".join(cvvals)
    vals = ", ".join(vvals)

    padding = "\n" + " " * 16
    pack_stmts = padding.join(packs)
    unpack_stmts = padding.join(unpacks)
    # unpack_into_stmts = padding.join(unpack_intos)

    if not for_each_nested:
        for_each_nested_stmt = "_for_each_nested = None"
    else:
        for_each_nested_stmt = f"def _for_each_nested(fn):"
        for_each_nested_stmt += "\n" + " " * 12
        for_each_nested_stmt += f"try:{padding}"
        for_each_nested_stmt += padding.join(for_each_nested)
        for_each_nested_stmt += "\n" + " " * 12
        for_each_nested_stmt += f"except Exception as e:"
        for_each_nested_stmt += (
            f"{padding}raise ValueError(f'{err_name}: error in for_each_nested') from e"
        )

    ctx["_s"] = s

    # Construct the serialization functions using the same hack NamedTuple uses
    fnsrc = inspect.cleandoc(f"""
        from wpiutil import wpistruct

        def _pack(v):
            try:
                {pack_stmts}
                return _s.pack({vals})
            except Exception as e:
                raise ValueError(f"{err_name}: error packing data") from e
                            
        def _pack_into(v, b):
            try:
                {pack_stmts}
                return _s.pack_into(b, 0, {vals})
            except Exception as e:
                raise ValueError(f"{err_name}: error packing data") from e

        def _unpack(b):
            try:
                {uvals} = _s.unpack(b)
                {unpack_stmts}
                return cls({cvals})
            except Exception as e:
                raise ValueError(f"{err_name}: error unpacking data") from e
        
        #def _unpack_into(v, b):
        #    try:
        #        {vals} = _s.unpack(b)
        #        {{unpack_into_stmts}}
        #    except Exception as e:
        #        raise ValueError(f"{err_name}: error unpacking data") from e

        {for_each_nested_stmt}
    """)

    exec(fnsrc, ctx, ctx)

    cls.WPIStruct = StructDescriptor(
        typename=struct_name,
        schema="; ".join(schema),
        size=s.size,
        pack=ctx["_pack"],
        pack_into=ctx["_pack_into"],
        unpack=ctx["_unpack"],
        # unpack_into=ctx["_unpack_into"],
        for_each_nested=ctx["_for_each_nested"],
    )

    return cls
