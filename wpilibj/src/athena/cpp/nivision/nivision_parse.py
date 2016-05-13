from __future__ import print_function

import re
import traceback

__all__ = ["define_after_struct", "defined", "forward_structs", "opaque_structs", "enums",
           "structs", "prescan_file", "parse_file", "number_re", "constant_re"]

# parser regular expressions
number_re = re.compile(r'-?[0-9]+')
constant_re = re.compile(r'[A-Z0-9_]+')
define_re = re.compile(r'^#define\s+(?P<name>(IMAQ|ERR)[A-Z0-9_]+)\s+(?P<value>.*)')
enum_re = re.compile(r'^typedef\s+enum\s+(?P<name>[A-Za-z0-9]+)_enum\s*{')
enum_value_re = re.compile(r'^\s*(?P<name>[A-Za-z0-9_]+)\s*(=\s*(?P<value>-?[0-9A-Fx]+))?\s*,?')
struct_re = re.compile(r'^typedef\s+struct\s+(?P<name>[A-Za-z0-9]+)_struct\s*{')
union_re = re.compile(r'^typedef\s+union\s+(?P<name>[A-Za-z0-9]+)_union\s*{')
func_pointer_re = re.compile(
    r'\s*(?P<restype>[A-Za-z0-9_*]+)\s*\(\s*[A-Za-z0-9_]*\s*[*]\s*(?P<name>[A-Za-z0-9_]+)\s*\)\s*\((?P<params>[^)]*)\)')
static_const_re = re.compile(
    r'^static\s+const\s+(?P<type>[A-Za-z0-9_]+)\s+(?P<name>[A-Za-z0-9_]+)\s*=\s*(?P<value>[^;]+);')
function_re = re.compile(
    r'^((IMAQ|NI)_FUNC\s+)?(?P<restype>(const\s+)?[A-Za-z0-9_*]+)\s+((IMAQ_STDCALL|NI_FUNC[C]?)\s+)?(?P<name>[A-Za-z0-9_]+)\s*\((?P<params>[^)]*)\);')

# defines deferred until after structures
define_after_struct = []
defined = set()
forward_structs = set()
opaque_structs = set()
enums = set()
structs = set()


def parse_cdecl(decl):
    decl = " ".join(decl.split())
    ctype, sep, name = decl.rpartition(' ')
    # look for array[]
    name, bracket, arr = name.partition('[')
    if arr:
        arr = arr[:-1]
    else:
        arr = None
    return name, ctype, arr


def split_comment(line):
    if line.startswith('/*'):
        return "", ""
    parts = line.split('//', 1)
    code = parts[0].strip()
    comment = parts[1].strip() if len(parts) > 1 else None
    return code, comment


def prescan_file(f):
    for line in f:
        code, comment = split_comment(line)
        if not code and not comment:
            continue

        # typedef struct {
        m = struct_re.match(code)
        if m is not None:
            structs.add(m.group('name'))
            continue

        # other typedef
        if code.startswith("typedef"):
            if '(' in code:
                continue
            name, typedef, arr = parse_cdecl(code[8:-1])
            if typedef.startswith("struct"):
                forward_structs.add(name)
            continue

    opaque_structs.update(forward_structs - structs)


def parse_file(emit, f, block_comment_exclude):
    in_block_comment = False
    cur_block = ""
    in_enum = None
    in_struct = None
    in_union = None

    for lineno, line in enumerate(f):
        code, comment = split_comment(line)
        if not code and not comment:
            continue
        # print(comment)

        # in block comment
        if in_block_comment:
            if not code and comment is not None and comment[0] == '=':
                # closing block comment; emit if not excluded
                if cur_block not in block_comment_exclude:
                    try:
                        emit.block_comment(cur_block)
                    except Exception as e:
                        print("%d: exception in block_comment():\n%s" % (
                            lineno + 1, traceback.format_exc()))
                in_block_comment = False
                # emit "after struct" constants in Globals
                if cur_block == "Globals":
                    for dname, dtext in define_after_struct:
                        try:
                            emit.text(dtext)
                        except Exception as e:
                            print("%d: exception in text():\n%s" % (
                                lineno + 1, traceback.format_exc()))
                        defined.add(dname)
                continue
            if not code and comment is not None:
                # remember current block
                cur_block = comment
            continue

        # inside enum
        if in_enum is not None:
            if code[0] == '}':
                # closing
                try:
                    emit.enum(*in_enum)
                except Exception as e:
                    print("%d: exception in enum():\n%s" % (lineno + 1, traceback.format_exc()))
                in_enum = None
                continue
            m = enum_value_re.match(code)
            if m is not None:
                in_enum[1].append((m.group('name'), m.group('value'), comment))
            continue

        # inside struct/union
        if in_struct is not None or in_union is not None:
            if code[0] == '}':
                # closing
                if in_struct is not None:
                    try:
                        emit.struct(*in_struct)
                    except Exception as e:
                        print("%d: exception in struct(\"%s\"):\n%s" % (
                            lineno + 1, in_struct[0], traceback.format_exc()))
                    in_struct = None
                if in_union is not None:
                    try:
                        emit.union(*in_union)
                    except Exception as e:
                        print("%d: exception in union(\"%s\"):\n%s" % (
                            lineno + 1, in_union[0], traceback.format_exc()))
                    in_union = None
                continue
            name, ctype, arr = parse_cdecl(code[:-1])
            # add to fields
            if in_struct is not None:
                in_struct[1].append((name, ctype, arr, comment))
            if in_union is not None:
                in_union[1].append((name, ctype, arr, comment))
            continue

        # block comment
        if not code and comment is not None and comment[0] == '=':
            in_block_comment = True

        # #define
        m = define_re.match(code)
        if m is not None:
            try:
                emit.define(m.group('name'), m.group('value').strip(), comment)
            except Exception as e:
                print("%d: exception in define():\n%s" % (lineno + 1, traceback.format_exc()))
            continue

        # typedef enum {
        m = enum_re.match(code)
        if m is not None:
            in_enum = (m.group('name'), [])
            continue

        # typedef struct {
        m = struct_re.match(code)
        if m is not None:
            in_struct = (m.group('name'), [])
            continue

        # typedef union {
        m = union_re.match(code)
        if m is not None:
            in_union = (m.group('name'), [])
            continue

        # other typedef
        if code.startswith("typedef"):
            # typedef function?
            m = func_pointer_re.match(code[8:-1])
            if m is not None:
                params = [parse_cdecl(param.strip()) for param in
                          m.group('params').strip().split(',') if param.strip()]
                try:
                    emit.typedef_function(m.group('name'), m.group('restype'), params)
                except Exception as e:
                    print("%d: exception in typedef_function():\n%s" % (
                        lineno + 1, traceback.format_exc()))
                continue
            if '(' in code:
                print("Invalid typedef: %s" % code)
                continue
            emit.typedef(*parse_cdecl(code[8:-1]))
            continue

        # function
        m = function_re.match(code)
        if m is not None:
            params = [parse_cdecl(param.strip()) for param in m.group('params').strip().split(',')
                      if param.strip()]
            try:
                emit.function(m.group('name'), m.group('restype'), params)
            except Exception as e:
                print("%d: exception in function(\"%s\"):\n%s" % (
                    lineno + 1, m.group('name'), traceback.format_exc()))
            continue

        # static const
        m = static_const_re.match(code)
        if m is not None:
            value = m.group('value')
            if value[0] == '{':
                value = [v.strip() for v in value[1:-1].strip().split(',') if v.strip()]
            try:
                emit.static_const(m.group('name'), m.group('type'), value)
            except Exception as e:
                print("%d: exception in static_const():\n%s" % (lineno + 1, traceback.format_exc()))
            continue

        if not code or code[0] == '#':
            continue

        if not code or code[0] == '#':
            continue

        if code == 'extern "C" {' or code == "}":
            continue

        print("%d: Unrecognized: %s" % (lineno + 1, code))
