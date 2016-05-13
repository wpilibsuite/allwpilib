from __future__ import print_function

import codecs
import os
import sys

try:
    import configparser
except ImportError:
    import ConfigParser as configparser

from nivision_parse import *

# base, cast-out-pre, cast-out-post, cast-in-pre, cast-in-post
java_accessor_map = {
    "B": ("", "", "", "", ""),
    "C": ("Char", "", "", "", ""),
    "S": ("Short", "", "", "", ""),
    "I": ("Int", "", "", "", ""),
    "J": ("Long", "", "", "", ""),
    "F": ("Float", "", "", "", ""),
    "D": ("Double", "", "", "", ""),
    "Z": ("Boolean", "", "", "", ""),
    "X": ("", "(short)(", " & 0xff)", "(byte)(", " & 0xff)"),
    "Y": ("Short", "(int)(", " & 0xffff)", "(short)(", " & 0xffff)"),
}

java_size_map = {
    "B": 1,
    "C": 2,
    "S": 2,
    "I": 4,
    "J": 8,
    "F": 4,
    "D": 8,
    "Z": 1,
}


class JavaType:
    def __init__(self, j_type, jn_type, jni_type, jni_sig, is_enum=False, is_struct=False,
                 is_opaque=False, string_array=False, array_size=None):
        self.j_type = j_type
        self.jn_type = jn_type
        self.jni_type = jni_type
        self.jni_sig = jni_sig
        self.is_enum = is_enum
        self.is_struct = is_struct
        self.is_opaque = is_opaque
        self.string_array = string_array
        self.array_size = array_size

    def copy(self):
        return JavaType(self.j_type, self.jn_type, self.jni_type, self.jni_sig,
                        is_enum=self.is_enum, is_struct=self.is_struct,
                        is_opaque=self.is_opaque,
                        string_array=self.string_array,
                        array_size=self.array_size)

    def __repr__(self):
        return "JavaType(%s, %s, %s, %s, is_enum=%s, is_struct=%s, is_opaque=%s, string_array=%s, array_size=%s)" % (
            self.j_type, self.jn_type, self.jni_type, self.jni_sig,
            self.is_enum, self.is_struct,
            self.is_opaque, self.string_array, self.array_size)


java_types_map = {
    ("void", None): JavaType("void", "void", "void", None),
    ("env", None): JavaType("", "", "JNIEnv*", None),
    ("cls", None): JavaType("", "", "jclass", None),
    ("int", None): JavaType("int", "int", "jint", "I"),
    ("char", None): JavaType("byte", "byte", "jbyte", "B"),
    ("wchar_t", None): JavaType("char", "char", "jchar", "C"),
    ("unsigned char", None): JavaType("short", "short", "jshort", "X"),
    ("short", None): JavaType("short", "short", "jshort", "S"),
    ("unsigned short", None): JavaType("int", "int", "jint", "Y"),
    ("unsigned", None): JavaType("int", "int", "jint", "I"),
    ("unsigned int", None): JavaType("int", "int", "jint", "I"),
    ("uInt32", None): JavaType("int", "int", "jint", "I"),
    ("IMAQdxSession", None): JavaType("int", "int", "jint", "I"),
    ("bool32", None): JavaType("int", "int", "jint", "I"),
    ("long", None): JavaType("long", "long", "jlong", "J"),
    ("unsigned long", None): JavaType("long", "long", "jlong", "J"),
    ("__int64", None): JavaType("long", "long", "jlong", "J"),
    ("long long", None): JavaType("long", "long", "jlong", "J"),
    ("unsigned __int64", None): JavaType("long", "long", "jlong", "J"),
    ("__uint64", None): JavaType("long", "long", "jlong", "J"),
    ("unsigned long long", None): JavaType("long", "long", "jlong", "J"),
    ("float", None): JavaType("float", "float", "jfloat", "F"),
    ("double", None): JavaType("double", "double", "jdouble", "D"),
    ("long double", None): JavaType("double", "double", "jdouble", "D"),
    ("unsigned char*", None): JavaType("String", "String", "jstring", "Ljava/lang/String;"),
    ("char*", None): JavaType("String", "String", "jstring", "Ljava/lang/String;"),
    ("void*", None): JavaType("RawData", "long", "jlong", "J", is_opaque=True),
    # ("size_t", None): JavaType("long", "long", "jlong", "J"),
    ("String255", None): JavaType("String", "String", "jstring", "Ljava/lang/String;",
                                  string_array=True, array_size="256"),
    ("String255", ""): JavaType("String[]", "String[]", "jstringArray", "[Ljava/lang/String;",
                                string_array=True, array_size="256"),
    ("char*", ""): JavaType("String[]", "String[]", "jstringArray", "[Ljava/lang/String;"),
    ("char", ""): JavaType("String", "String", "jstring", "Ljava/lang/String;", string_array=True,
                           array_size=""),
    ("unsigned char", ""): JavaType("byte[]", "byte[]", "jbyteArray", "[B"),
    ("short", ""): JavaType("short[]", "short[]", "jshortArray", "[S"),
    ("int", ""): JavaType("int[]", "int[]", "jintArray", "[I"),
    ("unsigned int", ""): JavaType("int[]", "int[]", "jintArray", "[I"),
    ("uInt32", ""): JavaType("int[]", "int[]", "jintArray", "[I"),
    ("long", ""): JavaType("long[]", "long[]", "jlongArray", "[J"),
    ("float", ""): JavaType("float[]", "float[]", "jfloatArray", "[F"),
    ("double", ""): JavaType("double[]", "double[]", "jdoubleArray", "[D"),
}


def c_to_jtype(name, arr):
    jtype = java_types_map.get((name, arr), None)
    if jtype is not None:
        return jtype

    # sized array is treated the same as unsized
    if arr is not None and arr != "":
        jtype = c_to_jtype(name, "").copy()
        jtype.array_size = arr
        java_types_map[(name, arr)] = jtype  # cache
        return jtype

    # Opaque structures
    if name in opaque_structs:
        if arr is None:
            jtype = JavaType(name, "long", "jlong", "J", is_opaque=True)
        else:
            # FIXME
            jtype = JavaType(name + "[]", "long[]", "jlongArray", "[J", is_opaque=True)
        java_types_map[(name, arr)] = jtype  # cache
        return jtype

    # Enums
    if name in enums:
        if arr is None:
            jtype = JavaType(name, "int", "jint", "I", is_enum=True)
        else:
            # FIXME
            jtype = JavaType(name + "[]", "int[]", "jintArray", "[I", is_enum=True)
        java_types_map[(name, arr)] = jtype  # cache
        return jtype

    # handle pointers as void* (FIXME)
    if name[-1] == '*':
        if name[:-1] not in structs and name[:-1] not in defined:
            return java_types_map[("void*", None)]
        return c_to_jtype(name[:-1], arr)

    # Otherwise it's a normal structure object
    if arr is None:
        jtype = JavaType(name, "long", "jlong", "J", is_struct=True)
    else:
        # FIXME
        jtype = JavaType(name + "[]", "long[]", "jlongArray", "[J", is_struct=True)
    java_types_map[(name, arr)] = jtype
    return jtype


class JavaEmitData:
    def __init__(self):
        self.construct = []
        self.backingRead = []
        self.read = []
        self.writeBufs = []
        self.write = []
        self.backingWrite = []
        self.toArg = ""

    def addConstruct(self, s):
        self.construct.extend(s.split('\n')[1 if s[0] == '\n' else 0:])

    def addBackingRead(self, s):
        self.backingRead.extend(s.split('\n')[1 if s[0] == '\n' else 0:])

    def addRead(self, s):
        self.read.extend(s.split('\n')[1 if s[0] == '\n' else 0:])

    def addWriteBuf(self, s):
        self.writeBufs.append(s)

    def addWrite(self, s):
        self.write.extend(s.split('\n')[1 if s[0] == '\n' else 0:])

    def addBackingWrite(self, s):
        self.backingWrite.extend(s.split('\n')[1 if s[0] == '\n' else 0:])


class JavaEmitArrayData(JavaEmitData):
    def __init__(self):
        JavaEmitData.__init__(self)
        self.addConstruct("{fname} = new {ftype_one}[0];")
        self.addBackingRead("int {size_fname} = {backing}.get{jaccessor}({size_foffset});")
        self.addBackingWrite("{backing}.put{jaccessor}({size_foffset}, {fname}.length);")


# sized array of null-terminated strings
strzArrayEmitSized = JavaEmitArrayData()
strzArrayEmitSized.addBackingRead("long {fname}_addr = getPointer({backing}, {foffset});")
strzArrayEmitSized.addRead("""
{fname} = new String[{size_fname}];
if ({size_fname} > 0 && {fname}_addr != 0) {{
    ByteBuffer bb = newDirectByteBuffer({fname}_addr, {size_fname}*{pointer_sz});
    for (int i=0, off=0; i<{size_fname}; i++, off += {pointer_sz}) {{
        long addr = getPointer(bb, off);
        if (addr == 0)
            {fname}[i] = null;
        else {{
            ByteBuffer bb2 = newDirectByteBuffer(addr, 1000); // FIXME
            while (bb2.get() != 0) {{}}
            byte[] bytes = new byte[bb2.position()-1];
            bb2.rewind();
            getBytes(bb2, bytes, 0, bytes.length);
            try {{
                {fname}[i] = new String(bytes, "UTF-8");
            }} catch (UnsupportedEncodingException e) {{
                {fname}[i] = "";
            }}
        }}
    }}
}}""")
strzArrayEmitSized.addWriteBuf("{buftype} {fname}_buf")
strzArrayEmitSized.addWriteBuf("{buftype}[] {fname}_bufs")
strzArrayEmitSized.addWrite("""
{fname}_buf = ByteBuffer.allocateDirect({fname}.length*{pointer_sz}).order(ByteOrder.nativeOrder());
for (int i=0, off=0; i<{fname}.length; i++, off += {pointer_sz}) {{
    if ({fname}[i] == null)
        putPointer({fname}_buf, off, 0);
    else {{
        byte[] bytes;
        try {{
            bytes = {fname}[i].getBytes("UTF-8");
        }} catch (UnsupportedEncodingException e) {{
            bytes = new byte[0];
        }}
        {fname}_bufs[i] = ByteBuffer.allocateDirect(bytes.length+1);
        putBytes({fname}_bufs[i], bytes, 0, bytes.length).put(bytes.length, (byte)0);
        putPointer({fname}_buf, off, getByteBufferAddress({fname}_bufs[i]));
    }}
}}""")
strzArrayEmitSized.addBackingWrite("putPointer({backing}, {foffset}, {fname}_buf);")

# unsized array; final terminating zero determines the length
strzArrayEmitUnsized = JavaEmitData()
strzArrayEmitUnsized.addConstruct("{fname} = new {ftype_one}[0];")
strzArrayEmitUnsized.addBackingRead("long {fname}_addr = getPointer({backing}, {foffset});")
strzArrayEmitUnsized.addRead("""
{{
    if ({fname}_addr == 0)
        {fname} = new {ftype_one}[0];
    else {{
        // prescan to find length
        ByteBuffer bb = newDirectByteBuffer({fname}_addr, 1000*{pointer_sz}); // FIXME
        int size = 0;
        for (int off=0; getPointer(bb, off) != 0; size++, off += {pointer_sz}) {{ }}
        {fname} = new String[size];
        for (int i=0, off=0; i<size; i++, off += {pointer_sz}) {{
            long addr = getPointer(bb, off);
            if (addr == 0)
                {fname}[i] = null;
            else {{
                ByteBuffer bb2 = newDirectByteBuffer(addr, 1000); // FIXME
                while (bb2.get() != 0) {{}}
                byte[] bytes = new byte[bb2.position()-1];
                bb2.rewind();
                getBytes(bb2, 0, bytes.length);
                try {{
                    {fname}[i] = new String(bytes, "UTF-8");
                }} catch (UnsupportedEncodingException e) {{
                    {fname}[i] = "";
                }}
            }}
        }}
    }}
}}""")
strzArrayEmitUnsized.addWriteBuf("{buftype} {fname}_buf")
strzArrayEmitUnsized.addWriteBuf("{buftype}[] {fname}_bufs")
strzArrayEmitUnsized.addWrite("""
{fname}_buf = ByteBuffer.allocateDirect(({fname}.length+1)*{pointer_sz}).order(ByteOrder.nativeOrder());
for (int i=0, off=0; i<{fname}.length; i++, off += {pointer_sz}) {{
    if ({fname}[i] == null)
        putPointer({fname}_buf, off, 0);
    else {{
        byte[] bytes;
        try {{
            bytes = {fname}[i].getBytes("UTF-8");
        }} catch (UnsupportedEncodingException e) {{
            bytes = new byte[0];
        }}
        {fname}_bufs[i] = ByteBuffer.allocateDirect(bytes.length+1);
        putBytes({fname}_bufs[i], bytes, 0, bytes.length).put(bytes.length, (byte)0);
        putPointer({fname}_buf, off, getByteBufferAddress({fname}_bufs[i]));
    }}
}}
putPointer({fname}_buf, {fname}.length*{pointer_sz}, 0); // terminator""")
strzArrayEmitUnsized.addBackingWrite("putPointer({backing}, {foffset}, {fname}_buf);")

# array of enum values
enumArrayEmit = JavaEmitArrayData()
enumArrayEmit.addBackingRead("long {fname}_addr = getPointer({backing}, {foffset});")
enumArrayEmit.addRead("""
{fname} = new {ftype_one}[{size_fname}];
if ({size_fname} > 0 && {fname}_addr != 0) {{
    ByteBuffer bb = newDirectByteBuffer({fname}_addr, {size_fname}*%d);
    for (int i=0, off=0; i<{size_fname}; i++, off += %d) {{
        {fname}[i] = {ftype_one}.fromValue(bb.getInt(off));
    }}
}}""" % (4, 4))
enumArrayEmit.addWriteBuf("{buftype} {fname}_buf")
enumArrayEmit.addWrite("""
{fname}_buf = ByteBuffer.allocateDirect({fname}.length*%d).order(ByteOrder.nativeOrder());
for (int i=0, off=0; i<{fname}.length; i++, off += %d) {{
    if ({fname} != null)
        {fname}_buf.putInt(off, {fname}[i].getValue());
}}""" % (4, 4))
enumArrayEmit.addBackingWrite("putPointer({backing}, {foffset}, {fname}_buf);")
enumArrayEmit.toArg = "getByteBufferAddress({fname}_buf)"

# array of opaque structures
opaqueArrayEmit = JavaEmitArrayData()
opaqueArrayEmit.addBackingRead("long {fname}_addr = getPointer({backing}, {foffset});")
opaqueArrayEmit.addRead("""
{fname} = new {ftype_one}[{size_fname}];
if ({size_fname} > 0 && {fname}_addr != 0) {{
    ByteBuffer bb = newDirectByteBuffer({fname}_addr, {size_fname}*{pointer_sz});
    for (int i=0, off=0; i<{size_fname}; i++, off += {pointer_sz}) {{
        {fname}[i] = new {ftype_one}(getPointer(bb, off), false);
    }}
}}""")
opaqueArrayEmit.addWriteBuf("{buftype} {fname}_buf")
opaqueArrayEmit.addWrite("""
{fname}_buf = ByteBuffer.allocateDirect({fname}.length*{pointer_sz}).order(ByteOrder.nativeOrder());
for (int i=0, off=0; i<{fname}.length; i++, off += {pointer_sz}) {{
    putPointer({fname}_buf, off, {fname}[i]);
}}""")
opaqueArrayEmit.addBackingWrite("putPointer({backing}, {foffset}, {fname}_buf);")
opaqueArrayEmit.toArg = "getByteBufferAddress({fname}_buf)"

# array of String255
string255ArrayEmit = JavaEmitArrayData()
string255ArrayEmit.addBackingRead("""
{fname} = new String[{size_fname}];
if ({size_fname} > 0) {{
    byte[] bytes = new byte[%d];
    int len;
    for (int i=0, off={foffset}; i<{size_fname}; i++, off += %d) {{
        getBytes({backing}, bytes, off, %d);
        for (len=0; len<bytes.length && bytes[len] != 0; len++) {{}}
        try {{
            {fname}[i] = new String(bytes, 0, len, "UTF-8");
        }} catch (UnsupportedEncodingException e) {{
            {fname}[i] = "";
        }}
    }}
}}""" % (256, 256, 256))
string255ArrayEmit.addBackingWrite("""
for (int i=0, off={foffset}; i<{size_fname}; i++, off += %d) {{
    byte[] bytes;
    try {{
        bytes = {fname}.getBytes("UTF-8");
    }} catch (UnsupportedEncodingException e) {{
        bytes = new byte[0];
    }}
    putBytes({backing}, bytes, off, bytes.length);
    for (int i=bytes.length; i<%d; i++)
        {backing}.put(off+i, (byte)0); // fill with zero
}}""" % (256, 256))

# array of normal structures
structArrayEmit = JavaEmitArrayData()
structArrayEmit.addBackingRead("long {fname}_addr = getPointer({backing}, {foffset});")
structArrayEmit.addRead("""
{fname} = new {ftype_one}[{size_fname}];
if ({size_fname} > 0 && {fname}_addr != 0) {{
    ByteBuffer bb = newDirectByteBuffer({fname}_addr, {size_fname}*{struct_sz});
    for (int i=0, off=0; i<{size_fname}; i++, off += {struct_sz}) {{
        {fname}[i] = new {ftype_one}(bb, off);
        {fname}[i].read();
    }}
}}""")
structArrayEmit.addWriteBuf("{buftype} {fname}_buf")
# FIXME: This can be optimized for the read->write case.
structArrayEmit.addWrite("""
{fname}_buf = ByteBuffer.allocateDirect({fname}.length*{struct_sz}).order(ByteOrder.nativeOrder());
for (int i=0, off=0; i<{fname}.length; i++, off += {struct_sz}) {{
    {fname}[i].setBuffer({fname}_buf, off);
    {fname}[i].write();
}}""")
structArrayEmit.addBackingWrite("putPointer({backing}, {foffset}, {fname}_buf);")
structArrayEmit.toArg = "getByteBufferAddress({fname}_buf)"

# array of bytes
byteArrayEmit = JavaEmitArrayData()
byteArrayEmit.addBackingRead("long {fname}_addr = getPointer({backing}, {foffset});")
byteArrayEmit.addRead("""
{fname} = new {ftype_one}[{size_fname}];
if ({size_fname} > 0 && {fname}_addr != 0) {{
    getBytes(newDirectByteBuffer({fname}_addr, {size_fname}), {fname}, 0, {size_fname});
}}""")
byteArrayEmit.addWriteBuf("{buftype} {fname}_buf")
byteArrayEmit.addWrite("""
{fname}_buf = ByteBuffer.allocateDirect({fname}.length);
putBytes({fname}_buf, {fname}, 0, {fname}.length);""")
byteArrayEmit.addBackingWrite("putPointer({backing}, {foffset}, {fname}_buf);")
byteArrayEmit.toArg = "getByteBufferAddress({fname}_buf)"

# array of java types
jtypeArrayEmit = JavaEmitArrayData()
jtypeArrayEmit.addBackingRead("long {fname}_addr = getPointer({backing}, {foffset});")
jtypeArrayEmit.addRead("""{fname} = new {ftype_one}[{size_fname}];
if ({size_fname} > 0 && {fname}_addr != 0) {{
    newDirectByteBuffer({fname}_addr, {size_fname}*{struct_sz}).as{buftype}().get({fname});
}}""")
jtypeArrayEmit.addWriteBuf("ByteBuffer {fname}_buf")
jtypeArrayEmit.addWrite("""
{fname}_buf = ByteBuffer.allocateDirect({fname}.length*{struct_sz}).order(ByteOrder.nativeOrder());
{fname}_buf.as{buftype}().put({fname}).rewind();""")
jtypeArrayEmit.addBackingWrite("putPointer({backing}, {foffset}, {fname}_buf);")
jtypeArrayEmit.toArg = "getByteBufferAddress({fname}_buf)"

# enum
enumEmit = JavaEmitData()
enumEmit.addBackingRead("{fname} = {ftype}.fromValue({backing}.getInt({foffset}));")
enumEmit.addBackingWrite("""if ({fname} != null)
    {backing}.putInt({foffset}, {fname}.getValue());""")
enumEmit.toArg = "{fname}.getValue()"

# opaque structure
opaqueEmit = JavaEmitData()
opaqueEmit.addBackingRead("long {fname}_addr = getPointer({backing}, {foffset});")
opaqueEmit.addRead("""if ({fname}_addr == 0)
    {fname} = null;
else
    {fname} = new {ftype}({fname}_addr, false);""")
opaqueEmit.addBackingWrite("putPointer({backing}, {foffset}, {fname});")
opaqueEmit.toArg = "{fname}.getAddress()"

# inline normal structure
structEmit = JavaEmitData()
structEmit.addConstruct("{fname} = new {ftype}({backing}, {foffset});")
structEmit.addRead("{fname}.read();")
structEmit.addWrite("{fname}.write();")
structEmit.toArg = "{fname}.getAddress()"

# java type
jtypeEmit = JavaEmitData()
jtypeEmit.addBackingRead(
    "{fname} = {jaccessor_cast_out_pre}{backing}.get{jaccessor}({foffset}){jaccessor_cast_out_post};")
jtypeEmit.addBackingWrite(
    "{backing}.put{jaccessor}({foffset}, {jaccessor_cast_in_pre}{fname}{jaccessor_cast_in_post});")

# string - array of characters
strSizedEmit = JavaEmitData()
strSizedEmit.addBackingRead("""{{
    byte[] bytes = new byte[{array_size}];
    getBytes({backing}, bytes, {foffset}, {array_size});
    int len;
    for (len=0; len<bytes.length && bytes[len] != 0; len++) {{}}
    try {{
        {fname} = new String(bytes, 0, len, "UTF-8");
    }} catch (UnsupportedEncodingException e) {{
        {fname} = "";
    }}
}}""")
strSizedEmit.addBackingWrite("""
if ({fname} != null) {{
    byte[] bytes;
    try {{
        bytes = {fname}.getBytes("UTF-8");
    }} catch (UnsupportedEncodingException e) {{
        bytes = new byte[0];
    }}
    putBytes({backing}, bytes, {foffset}, bytes.length);
    for (int i=bytes.length; i<{array_size}; i++)
        {backing}.put(i, (byte)0); // fill with zero
}}""")

# null terminated string
strzEmit = JavaEmitData()
strzEmit.addBackingRead("long {fname}_addr = getPointer({backing}, {foffset});")
strzEmit.addRead("""if ({fname}_addr == 0)
    {fname} = null;
else {{
    ByteBuffer bb = newDirectByteBuffer({fname}_addr, 1000); // FIXME
    while (bb.get() != 0) {{}}
    byte[] bytes = new byte[bb.position()-1];
    getBytes(bb, bytes, 0, bytes.length);
    try {{
        {fname} = new String(bytes, "UTF-8");
    }} catch (UnsupportedEncodingException e) {{
        {fname} = "";
    }}
}}
""")
strzEmit.addWriteBuf("{buftype} {fname}_buf")
strzEmit.addWrite("""
if ({fname} != null) {{
    byte[] {fname}_bytes;
    try {{
        {fname}_bytes = {fname}.getBytes("UTF-8");
    }} catch (UnsupportedEncodingException e) {{
        {fname}_bytes = new byte[0];
    }}
    {fname}_buf = ByteBuffer.allocateDirect({fname}_bytes.length+1);
    putBytes({fname}_buf, {fname}_bytes, 0, {fname}_bytes.length).put({fname}_bytes.length, (byte)0);
}}""")
strzEmit.addBackingWrite(
    "putPointer({backing}, {foffset}, {fname} == null ? 0 : getByteBufferAddress({fname}_buf));")
strzEmit.toArg = "{fname} == null ? 0 : getByteBufferAddress({fname}_buf)"


class JavaStructEmitHelper:
    def __init__(self, emit, name, fields, sized_members=None):
        self.emit = emit
        self.name = name
        self.fields = fields

        self.exclude_members = set(self.config_get("exclude_members", "").split(','))
        self.exclude_members |= set(
            x.split(':')[0] for x in self.config_get("uniontype", "").split(','))
        self.union_members = {}
        for v in self.config_get("uniontype", "").split(','):
            if not v:
                continue
            vl = v.split(':')
            self.union_members[vl[0]] = (
                vl[1], [tuple(y.strip() for y in x.split('=')) for x in vl[2:]])

        if sized_members is not None:
            self.sized_members = sized_members
        else:
            self.sized_members = dict(tuple(y.strip() for y in x.split(':')) for x in
                                      self.config_get("arraysize", "").split(',') if x)
        self.size_members = dict((x, None) for x in self.sized_members.values())

        # get type of each sized member
        for fname, ftype, arr, comment in fields:
            if fname in self.size_members:
                self.size_members[fname] = ftype

    def config_get(self, option, fallback):
        return self.emit.config_get(self.name, option, fallback)

    def config_getboolean(self, section, option, fallback):
        return self.emit.config_getboolean(self.name, option, fallback)

    def config_struct_get(self, option):
        return self.emit.config_struct.get(self.name, option)

    def get_field_java_code(self, fname, ftype, arr, foffset, jfielddefs_private,
                            backing="backing"):
        """Returns dict of fielddef, init, read, write, type
        """
        if ftype.startswith("const"):
            ftype = ftype[5:].strip()
        if fname in self.sized_members:
            # Change from pointer to array
            if ftype[-1] == '*':
                ftype = ftype[:-1]
            arr = ""
            size_fname = self.sized_members[fname]
            size_jtype = c_to_jtype(self.size_members[size_fname], None)
            size_foffset = self.config_struct_get(size_fname)
        else:
            size_fname = ""
            size_jtype = None
            size_foffset = None

        is_pointer = False
        if ftype[-1] == '*' and ftype[:-1] in opaque_structs:
            # silently strip pointer from native structs
            ftype = ftype[:-1]
        elif ftype[-1] == '*' and (ftype, arr) not in java_types_map:
            # Not an array, but not inline either
            ftype = ftype[:-1]
            is_pointer = True

        # Hopefully the base type now
        jtype = c_to_jtype(ftype, arr)

        struct_sz = None
        buftype = "ByteBuffer"
        array_size = jtype.array_size or ""
        jaccessor = (None, None, None, None, None)

        writeBufs = []
        backingRead = []
        read = []
        write = []
        backingWrite = []
        construct = []
        toArg = ""

        typeemit = None
        if jtype.jni_sig == "[Ljava/lang/String;":
            # null-terminated strings
            if size_fname:
                jaccessor = java_accessor_map[size_jtype.jni_sig[0]]
                typeemit = strzArrayEmitSized
            else:
                typeemit = strzArrayEmitUnsized
        elif jtype.jni_sig[0] == '[':
            if is_pointer: raise NotImplementedError("pointer to array")
            if arr:
                construct.append("{fname} = new {ftype_one}[%s];" % arr)
                if jtype.is_opaque or jtype.is_enum:
                    raise NotImplementedError("array of opaque and enum not implemented")
                elif jtype.is_struct:
                    struct_sz = self.emit.config_struct.get(jtype.j_type[:-2], "_SIZE_")
                    construct.extend(("""
for (int i=0, off={foffset}; i<%s; i++, off += {struct_sz})
    {fname}[i] = new {ftype_one}({backing}, off);""" % arr).split('\n'))
                    read.extend(("""for ({ftype_one} it : {fname}) {{
    it.read();
}}""").split('\n'))
                    write.extend(("""for ({ftype_one} it : {fname}) {{
    it.write();
}}""").split('\n'))
                else:
                    raise NotImplementedError("sized array of unknown type")
            else:
                jaccessor = java_accessor_map[size_jtype.jni_sig[0]]
                if jtype.is_enum:
                    typeemit = enumArrayEmit
                elif jtype.is_opaque:
                    typeemit = opaqueArrayEmit
                elif ftype == "String255":
                    typeemit = string255ArrayEmit
                elif jtype.is_struct:
                    struct_sz = self.emit.config_struct.get(jtype.j_type[:-2], "_SIZE_")
                    typeemit = structArrayEmit
                elif jtype.jni_sig[1] == 'B':
                    typeemit = byteArrayEmit
                elif jtype.jni_sig[1] in java_accessor_map:
                    buftype = "%sBuffer" % java_accessor_map[jtype.jni_sig[1]][0]
                    struct_sz = java_size_map[jtype.jni_sig[1]]
                    typeemit = jtypeArrayEmit
                else:
                    raise ValueError("unrecognized jni signature '%s'" % jtype.jni_sig)
        elif jtype.is_enum:
            if is_pointer: raise NotImplementedError("pointer to enum")
            typeemit = enumEmit
        elif jtype.is_opaque or (jtype.is_struct and is_pointer):
            typeemit = opaqueEmit
        elif jtype.is_struct:
            typeemit = structEmit
        elif jtype.jni_sig[0] in java_accessor_map:
            if is_pointer: raise NotImplementedError("pointer to raw")
            jaccessor = java_accessor_map[jtype.jni_sig[0]]
            typeemit = jtypeEmit
        elif jtype.jni_sig == "Ljava/lang/String;":
            if is_pointer: raise NotImplementedError("pointer to string")
            if jtype.string_array:
                typeemit = strSizedEmit
            else:
                typeemit = strzEmit
        else:
            raise ValueError("unrecognized jni signature '%s'" % jtype.jni_sig)

        if typeemit is not None:
            writeBufs.extend(typeemit.writeBufs)
            construct.extend(typeemit.construct)
            backingRead.extend(typeemit.backingRead)
            read.extend(typeemit.read)
            write.extend(typeemit.write)
            backingWrite.extend(typeemit.backingWrite)
            toArg = typeemit.toArg

        for buf in writeBufs:
            jfielddefs_private.append("private %s;" % buf.format(buftype=buftype, fname=fname))
        jfielddef = 'public %s %s;' % (jtype.j_type, fname)

        fargs = dict(fname=fname,
                     ftype=jtype.j_type,
                     ftype_one=jtype.j_type[:-2],
                     foffset=foffset,
                     size_fname=fname + "_" + size_fname,
                     size_foffset=size_foffset,
                     pointer_sz=self.emit.config_struct.get("_platform_", "pointer"),
                     struct_sz=struct_sz,
                     array_size=array_size,
                     buftype=buftype,
                     jaccessor=jaccessor[0],
                     jaccessor_cast_out_pre=jaccessor[1],
                     jaccessor_cast_out_post=jaccessor[2],
                     jaccessor_cast_in_pre=jaccessor[3],
                     jaccessor_cast_in_post=jaccessor[4],
                     backing=backing)
        jconstruct = [x.format(**fargs) for x in construct]
        jwritebufs = [x.format(**fargs) for x in writeBufs]
        jbackingread = [x.format(**fargs) for x in backingRead]
        jread = [x.format(**fargs) for x in read]
        jwrite = [x.format(**fargs) for x in write]
        jbackingwrite = [x.format(**fargs) for x in backingWrite]
        jtoarg = toArg.format(**fargs)

        return dict(fielddef=jfielddef,
                    construct=jconstruct,
                    write_bufs=jwritebufs,
                    backing_read=jbackingread,
                    read=jread,
                    write=jwrite,
                    backing_write=jbackingwrite,
                    to_arg=jtoarg,
                    type=jtype,
                    is_pointer=is_pointer,
                    arr=arr,
                    size_fname=size_fname,
                    size_jtype=size_jtype,
                    size_foffset=size_foffset)

    def get_java_code(self):
        jcargs = []
        jcinit = []
        jfielddefs = []
        jfielddefs_private = []
        jconstruct = []
        jread = []
        jwrite = []

        # standard struct fields
        for fname, ftype, arr, comment in self.fields:
            if fname in self.size_members or fname in self.exclude_members:
                continue  # don't emit

            if ":" in fname:
                continue  # TODO: bit field
            foffset = self.config_struct_get(fname)
            field = self.get_field_java_code(fname, ftype, arr, foffset, jfielddefs_private)

            # XXX: hack to get short and float to work reasonably
            if field["type"].j_type == "short":
                jcargs.append("int {fname}".format(fname=fname))
                jcinit.append("this.{fname} = (short){fname};".format(fname=fname))
            elif field["type"].j_type == "float":
                jcargs.append("double {fname}".format(fname=fname))
                jcinit.append("this.{fname} = (float){fname};".format(fname=fname))
            else:
                jcargs.append("{ftype} {fname}".format(ftype=field["type"].j_type, fname=fname))
                jcinit.append("this.{fname} = {fname};".format(fname=fname))

            fielddef = field["fielddef"]
            if comment is not None:
                fielddef += ' // %s' % comment
            jfielddefs.append(fielddef)
            jconstruct.extend(field["construct"])
            jread.extend(field["backing_read"])
            jread.extend(field["read"])
            jwrite.extend(field["write"])
            jwrite.extend(field["backing_write"])

        # typed union fields
        jfielddefs_union_private = []
        for union_name, (switchvalue_name, members) in self.union_members.items():
            # get union structure name and type info
            for fname, ftype, arr, comment in self.fields:
                if fname == union_name:
                    union_ftype = ftype
                elif fname == switchvalue_name:
                    switchvalue_ftype = ftype
            unionfields = self.emit.unions[union_ftype]

            # build map of union fields for faster lookup
            ufieldmap = {}
            for fname, ftype, arr, comment in unionfields:
                ufieldmap[fname] = (ftype, arr, comment)

            # common offset
            foffset = self.config_struct_get(union_name)

            for enumval, fname in members:
                # find the rest of the info from the union fields
                ftype, arr, comment = ufieldmap[fname]

                field = self.get_field_java_code(fname, ftype, arr, foffset,
                                                 jfielddefs_union_private)

                fielddef = field["fielddef"]
                read = field["backing_read"]
                read.extend(field["read"])
                write = field["write"]
                write.extend(field["backing_write"])
                if comment is not None:
                    fielddef += ' // %s' % comment
                jfielddefs.append(fielddef)
                jconstruct.extend(field["construct"])
                if enumval.startswith("IMAQ_"):
                    enumval = enumval[5:]
                ifcheck = "if (%s == %s.%s) " % (switchvalue_name, switchvalue_ftype, enumval)
                if len(read[0]) > 0 and read[0][0] == '{':
                    read[0] = ifcheck + read[0]
                else:
                    read = ("    " + "\n    ".join(read)).split('\n')
                    read.insert(0, ifcheck + "{")
                    read.append("}")
                if len(write[0]) > 0 and write[0][0] == '{':
                    write[0] = ifcheck + write[0]
                else:
                    write = ("    " + "\n    ".join(write)).split('\n')
                    write.insert(0, ifcheck + "{")
                    write.append("}")

                jread.extend(read)
                jwrite.extend(write)

        jfielddefs.extend(jfielddefs_private)

        # Java definition
        p1 = """
    public static class {name} extends DisposedStruct {{
        {jfielddefs}

        private void init() {{
            {jconstruct}
        }}
        public {name}() {{
            super({size});
            init();
        }}"""

        if jcargs:
            p2 = """
        public {name}({jcargs}) {{
            super({size});
            {jcinit}
        }}"""
        else:
            p2 = ""

        p3 = """
        protected {name}(ByteBuffer backing, int offset) {{
            super(backing, offset, {size});
            init();
        }}
        protected {name}(long nativeObj, boolean owned) {{
            super(nativeObj, owned, {size});
            init();
        }}
        protected void setBuffer(ByteBuffer backing, int offset) {{
            super.setBuffer(backing, offset, {size});
        }}
        public void read() {{
            {jread}
        }}
        public void write() {{
            {jwrite}
        }}
        public int size() {{
            return {size};
        }}
    }}"""
        return "".join([p1, p2, p3]).format(
            name=self.name,
            jfielddefs="\n        ".join(jfielddefs),
            jread="\n            ".join(jread),
            jwrite="\n            ".join(jwrite),
            jconstruct="\n            ".join(jconstruct),
            jcargs=", ".join(jcargs),
            jcinit="\n            ".join(jcinit),
            size=self.config_struct_get("_SIZE_"))


class JavaEmitter:
    def __init__(self, outdir, config, config_struct, library_funcs):
        self.outdir = outdir
        self.config = config
        self.config_struct = config_struct
        self.library_funcs = library_funcs
        self.package = "com.ni.vision"
        self.classname = "NIVision"
        self.classpath = self.package.replace(".", "/") + "/" + self.classname

        self.unions = {}
        self.errors = {}

        with open(os.path.join(outdir, "VisionException.java"), "wt") as f:
            print("""//
// This file is auto-generated by wpilibj/wpilibJavaJNI/nivision/gen_java.py
// Please do not edit!
//

package {package};

public class VisionException extends RuntimeException {{

    private static final long serialVersionUID = 1L;

    public VisionException(String msg) {{
        super(msg);
    }}

    @Override
    public String toString() {{
        return "VisionException [" + super.toString() + "]";
    }}
}}""".format(package=self.package), file=f)

        self.out = open(os.path.join(outdir, "NIVision.java"), "wt")
        print("""//
// This file is auto-generated by wpilibj/wpilibJavaJNI/nivision/gen_java.py
// Please do not edit!
//

package {package};

import java.lang.reflect.*;
import java.io.UnsupportedEncodingException;
import java.nio.Buffer;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class {classname} {{
    private {classname}() {{}}

    private static native void imaqDispose(long addr);

    private static Constructor<?> constructDirectByteBuffer;
    private static Field bufferAddressField;

    static {{
        try {{
            Class<?>[] cArg = new Class[2];
            cArg[0] = long.class;
            cArg[1] = int.class;
            constructDirectByteBuffer = Class.forName("java.nio.DirectByteBuffer").getDeclaredConstructor(cArg);
            constructDirectByteBuffer.setAccessible(true);

            bufferAddressField = Buffer.class.getDeclaredField("address");
            bufferAddressField.setAccessible(true);
        }} catch (ReflectiveOperationException e) {{
            throw new ExceptionInInitializerError(e);
        }}
    }}

    private static ByteBuffer newDirectByteBuffer(long addr, int cap) {{
        try {{
            return ((ByteBuffer)(constructDirectByteBuffer.newInstance(addr, cap))).order(ByteOrder.nativeOrder());
        }} catch (ReflectiveOperationException e) {{
            throw new ExceptionInInitializerError(e);
        }}
    }}

    private static long getByteBufferAddress(ByteBuffer bb) {{
        try {{
            return bufferAddressField.getLong(bb);
        }} catch (IllegalAccessException e) {{
            return 0;
        }}
    }}

    public static ByteBuffer sliceByteBuffer(ByteBuffer bb, int offset, int size) {{
        int pos = bb.position();
        int lim = bb.limit();
        bb.position(offset);
        bb.limit(offset+size);
        ByteBuffer new_bb = bb.slice().order(ByteOrder.nativeOrder());
        bb.position(pos);
        bb.limit(lim);
        return new_bb;
    }}

    public static ByteBuffer getBytes(ByteBuffer bb, byte[] dst, int offset, int size) {{
        int pos = bb.position();
        bb.position(offset);
        bb.get(dst, 0, size);
        bb.position(pos);
        return bb;
    }}

    public static ByteBuffer putBytes(ByteBuffer bb, byte[] src, int offset, int size) {{
        int pos = bb.position();
        bb.position(offset);
        bb.put(src, 0, size);
        bb.position(pos);
        return bb;
    }}

    private static abstract class DisposedStruct {{
        protected ByteBuffer backing;
        private boolean owned;
        protected DisposedStruct(int size) {{
            backing = ByteBuffer.allocateDirect(size);
            backing.order(ByteOrder.nativeOrder());
            owned = false;
        }}
        protected DisposedStruct(ByteBuffer backing, int offset, int size) {{
            this.backing = sliceByteBuffer(backing, offset, size);
            owned = false;
        }}
        private DisposedStruct(long nativeObj, boolean owned, int size) {{
            backing = newDirectByteBuffer(nativeObj, size);
            this.owned = owned;
        }}
        public void free() {{
            if (owned) {{
                imaqDispose(getByteBufferAddress(backing));
                owned = false;
                backing = null;
            }}
        }}
        @Override
        protected void finalize() throws Throwable {{
            if (owned)
                imaqDispose(getByteBufferAddress(backing));
            super.finalize();
        }}
        public long getAddress() {{
            if (backing == null)
                return 0;
            write();
            return getByteBufferAddress(backing);
        }}
        protected void setBuffer(ByteBuffer backing, int offset, int size) {{
            this.backing = sliceByteBuffer(backing, offset, size);
        }}

        abstract public void read();
        abstract public void write();
        abstract public int size();
    }}

    private static abstract class OpaqueStruct {{
        private long nativeObj;
        private boolean owned;
        protected OpaqueStruct() {{
            nativeObj = 0;
            owned = false;
        }}
        protected OpaqueStruct(long nativeObj, boolean owned) {{
            this.nativeObj = nativeObj;
            this.owned = owned;
        }}
        public void free() {{
            if (owned && nativeObj != 0) {{
                imaqDispose(nativeObj);
                owned = false;
                nativeObj = 0;
            }}
        }}
        @Override
        protected void finalize() throws Throwable {{
            if (owned && nativeObj != 0)
                imaqDispose(nativeObj);
            super.finalize();
        }}
        public long getAddress() {{
            return nativeObj;
        }}
    }}

    public static class RawData {{
        private ByteBuffer buf;
        private boolean owned;
        public RawData() {{
            owned = false;
        }}
        public RawData(ByteBuffer buf) {{
            this.buf = buf;
            owned = false;
        }}
        private RawData(long nativeObj, boolean owned, int size) {{
            buf = newDirectByteBuffer(nativeObj, size);
            this.owned = owned;
        }}
        public void free() {{
            if (owned) {{
                imaqDispose(getByteBufferAddress(buf));
                owned = false;
                buf = null;
            }}
        }}
        @Override
        protected void finalize() throws Throwable {{
            if (owned)
                imaqDispose(getByteBufferAddress(buf));
            super.finalize();
        }}
        public long getAddress() {{
            if (buf == null)
                return 0;
            return getByteBufferAddress(buf);
        }}
        public ByteBuffer getBuffer() {{
            return buf;
        }}
        public void setBuffer(ByteBuffer buf) {{
            if (owned)
                free();
            this.buf = buf;
        }}
    }}""".format(package=self.package, classname=self.classname), file=self.out)

        if int(self.config_struct.get("_platform_", "pointer")) == 4:
            # 32-bit addressing
            java_types_map[("size_t", None)] = JavaType("int", "int", "jint", "I")
            print("""
    private static long getPointer(ByteBuffer bb, int offset) {
        return (long)bb.getInt(offset);
    }
    private static void putPointer(ByteBuffer bb, int offset, long address) {
        bb.putInt(offset, (int)address);
    }
    private static void putPointer(ByteBuffer bb, int offset, ByteBuffer buf) {
        if (buf == null)
            bb.putInt(offset, 0);
        else
            bb.putInt(offset, (int)getByteBufferAddress(buf));
    }
    private static void putPointer(ByteBuffer bb, int offset, DisposedStruct struct) {
        if (struct == null)
            bb.putInt(offset, 0);
        else
            bb.putInt(offset, (int)struct.getAddress());
    }
    private static void putPointer(ByteBuffer bb, int offset, OpaqueStruct struct) {
        if (struct == null)
            bb.putInt(offset, 0);
        else
            bb.putInt(offset, (int)struct.getAddress());
    }""", file=self.out)
        else:
            # 64-bit addressing
            java_types_map[("size_t", None)] = JavaType("long", "long", "jlong", "J")
            print("""
    private static long getPointer(ByteBuffer bb, int offset) {
        return bb.getLong(offset);
    }
    private static void putPointer(ByteBuffer bb, int offset, long address) {
        bb.putLong(offset, address);
    }
    private static void putPointer(ByteBuffer bb, int offset, ByteBuffer buf) {
        if (buf == null)
            bb.putLong(offset, 0);
        else
            bb.putLong(offset, getByteBufferAddress(buf));
    }
    private static void putPointer(ByteBuffer bb, int offset, OpaqueStruct struct) {
        if (struct == null)
            bb.putLong(offset, 0);
        else
            bb.putLong(offset, struct.getAddress());
    }
    private static void putPointer(ByteBuffer bb, int offset, DisposedStruct struct) {
        if (struct == null)
            bb.putLong(offset, 0);
        else
            bb.putLong(offset, struct.getAddress());
    }""", file=self.out)

        self.outc = open(os.path.join(outdir, "NIVision.cpp"), "wt")
        print("""//
// This file is auto-generated by wpilibj/wpilibJavaJNI/nivision/gen_java.py
// Please do not edit!
//

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <jni.h>
#include <nivision.h>
#include <NIIMAQdx.h>

static const char* getErrorText(int err);

// throw java exception
static void throwJavaException(JNIEnv *env) {{
    jclass je = env->FindClass("{packagepath}/VisionException");
    int err = imaqGetLastError();
    const char* err_text = getErrorText(err);
    char* full_err_msg = (char*)malloc(30+strlen(err_text));
    sprintf(full_err_msg, "imaqError: %d: %s", err, err_text);
    env->ThrowNew(je, full_err_msg);
    free(full_err_msg);
}}

// throw IMAQdx java exception
static void dxthrowJavaException(JNIEnv *env, IMAQdxError err) {{
    jclass je = env->FindClass("{packagepath}/VisionException");
    const char* err_text = getErrorText(err);
    char* full_err_msg = (char*)malloc(30+strlen(err_text));
    sprintf(full_err_msg, "IMAQdxError: %d: %s", err, err_text);
    env->ThrowNew(je, full_err_msg);
    free(full_err_msg);
}}

extern "C" {{

JNIEXPORT void JNICALL Java_{package}_{classname}_imaqDispose(JNIEnv* , jclass , jlong addr)
{{
    imaqDispose((void*)addr);
}}

static inline IMAQdxError NI_FUNC IMAQdxGetAttributeU32(IMAQdxSession id, const char* name, uInt32* value)
{{
    return IMAQdxGetAttribute(id, name, IMAQdxValueTypeU32, (void*)value);
}}
static inline IMAQdxError NI_FUNC IMAQdxGetAttributeI64(IMAQdxSession id, const char* name, Int64* value)
{{
    return IMAQdxGetAttribute(id, name, IMAQdxValueTypeI64, (void*)value);
}}
static inline IMAQdxError NI_FUNC IMAQdxGetAttributeF64(IMAQdxSession id, const char* name, float64* value)
{{
    return IMAQdxGetAttribute(id, name, IMAQdxValueTypeF64, (void*)value);
}}
static inline IMAQdxError NI_FUNC IMAQdxGetAttributeString(IMAQdxSession id, const char* name, char value[IMAQDX_MAX_API_STRING_LENGTH])
{{
    return IMAQdxGetAttribute(id, name, IMAQdxValueTypeString, (void*)value);
}}
static inline IMAQdxError NI_FUNC IMAQdxGetAttributeEnum(IMAQdxSession id, const char* name, IMAQdxEnumItem* value)
{{
    return IMAQdxGetAttribute(id, name, IMAQdxValueTypeEnumItem, (void*)value);
}}
static inline IMAQdxError NI_FUNC IMAQdxGetAttributeBool(IMAQdxSession id, const char* name, bool32* value)
{{
    return IMAQdxGetAttribute(id, name, IMAQdxValueTypeBool, (void*)value);
}}

static inline IMAQdxError NI_FUNC IMAQdxGetAttributeMinimumU32(IMAQdxSession id, const char* name, uInt32* value)
{{
    return IMAQdxGetAttributeMinimum(id, name, IMAQdxValueTypeU32, (void*)value);
}}
static inline IMAQdxError NI_FUNC IMAQdxGetAttributeMinimumI64(IMAQdxSession id, const char* name, Int64* value)
{{
    return IMAQdxGetAttributeMinimum(id, name, IMAQdxValueTypeI64, (void*)value);
}}
static inline IMAQdxError NI_FUNC IMAQdxGetAttributeMinimumF64(IMAQdxSession id, const char* name, float64* value)
{{
    return IMAQdxGetAttributeMinimum(id, name, IMAQdxValueTypeF64, (void*)value);
}}

static inline IMAQdxError NI_FUNC IMAQdxGetAttributeMaximumU32(IMAQdxSession id, const char* name, uInt32* value)
{{
    return IMAQdxGetAttributeMaximum(id, name, IMAQdxValueTypeU32, (void*)value);
}}
static inline IMAQdxError NI_FUNC IMAQdxGetAttributeMaximumI64(IMAQdxSession id, const char* name, Int64* value)
{{
    return IMAQdxGetAttributeMaximum(id, name, IMAQdxValueTypeI64, (void*)value);
}}
static inline IMAQdxError NI_FUNC IMAQdxGetAttributeMaximumF64(IMAQdxSession id, const char* name, float64* value)
{{
    return IMAQdxGetAttributeMaximum(id, name, IMAQdxValueTypeF64, (void*)value);
}}

static inline IMAQdxError NI_FUNC IMAQdxGetAttributeIncrementU32(IMAQdxSession id, const char* name, uInt32* value)
{{
    return IMAQdxGetAttributeIncrement(id, name, IMAQdxValueTypeU32, (void*)value);
}}
static inline IMAQdxError NI_FUNC IMAQdxGetAttributeIncrementI64(IMAQdxSession id, const char* name, Int64* value)
{{
    return IMAQdxGetAttributeIncrement(id, name, IMAQdxValueTypeI64, (void*)value);
}}
static inline IMAQdxError NI_FUNC IMAQdxGetAttributeIncrementF64(IMAQdxSession id, const char* name, float64* value)
{{
    return IMAQdxGetAttributeIncrement(id, name, IMAQdxValueTypeF64, (void*)value);
}}

static inline IMAQdxError NI_FUNC IMAQdxSetAttributeU32(IMAQdxSession id, const char* name, uInt32 value)
{{
    return IMAQdxSetAttribute(id, name, IMAQdxValueTypeU32, value);
}}
static inline IMAQdxError NI_FUNC IMAQdxSetAttributeI64(IMAQdxSession id, const char* name, Int64 value)
{{
    return IMAQdxSetAttribute(id, name, IMAQdxValueTypeI64, value);
}}
static inline IMAQdxError NI_FUNC IMAQdxSetAttributeF64(IMAQdxSession id, const char* name, float64 value)
{{
    return IMAQdxSetAttribute(id, name, IMAQdxValueTypeF64, value);
}}
static inline IMAQdxError NI_FUNC IMAQdxSetAttributeString(IMAQdxSession id, const char* name, const char* value)
{{
    return IMAQdxSetAttribute(id, name, IMAQdxValueTypeString, value);
}}
static inline IMAQdxError NI_FUNC IMAQdxSetAttributeEnum(IMAQdxSession id, const char* name, const IMAQdxEnumItem* value)
{{
    return IMAQdxSetAttribute(id, name, IMAQdxValueTypeU32, value->Value);
}}
static inline IMAQdxError NI_FUNC IMAQdxSetAttributeBool(IMAQdxSession id, const char* name, bool32 value)
{{
    return IMAQdxSetAttribute(id, name, IMAQdxValueTypeBool, value);
}}
""".format(packagepath=self.package.replace(".", "/"),
           package=self.package.replace(".", "_"),
           classname=self.classname), file=self.outc)

        self.block_comment("Opaque Structures")
        for name in sorted(opaque_structs):
            self.opaque_struct(name)

    def finish(self):
        print("}", file=self.out)
        print("""}}

static const char* getErrorText(int err) {{
    switch (err) {{
        {errs}
        default: return "Unknown error";
    }}
}}""".format(errs="\n        ".join(
            'case %s: return "%s";' % (x, self.errors[x]) for x in sorted(self.errors))),
            file=self.outc)

    def config_get(self, section, option, fallback):
        try:
            return self.config.get(section, option)
        except (ValueError, configparser.NoSectionError, configparser.NoOptionError):
            return fallback

    def config_getboolean(self, section, option, fallback):
        try:
            return self.config.getboolean(section, option)
        except (ValueError, configparser.NoSectionError, configparser.NoOptionError):
            return fallback

    def block_comment(self, comment):
        print("""
    /**
     * {comment}
     */""".format(comment=comment), file=self.out)
        print("""
/*
 * {comment}
 */""".format(comment=comment), file=self.outc)

    def opaque_struct(self, name):
        print("""
    public static class {name} extends OpaqueStruct {{
        private {name}() {{}}
        private {name}(long nativeObj, boolean owned) {{
            super(nativeObj, owned);
        }}
    }}""".format(name=name), file=self.out)

    def define(self, name, value, comment):
        if self.config_getboolean(name, "exclude", fallback=False):
            return
        if name in opaque_structs:
            return
        clean = None
        type = None
        after_struct = False
        if value == "TRUE":
            clean = "true"
            type = "boolean"
        elif value == "FALSE":
            clean = "false"
            type = "boolean"
        elif name.startswith("IMAQ_INIT_RGB") and value[0] == '{' and value[-1] == '}':
            return
        elif value.startswith("imaqMake"):
            clean = "new " + value[8:]
            type = value.split("(")[0][8:]
            after_struct = True
        elif value[0] == '"':
            if len(value) == 2:
                clean = "{ 0 }"
            else:
                clean = "{ %s,0 }" % ",".join("0x%x" % ord(c) for c in value[1:-1])
            type = "byte[]"
        elif number_re.match(value):
            clean = value
            type = "int"
        elif constant_re.match(value):
            clean = value
            after_struct = value not in defined

        if clean is None:
            print("Invalid #define: %s" % name)
            return

        if name.startswith("ERR_"):
            self.errors[name] = comment
            return

        # strip IMAQ_ prefix
        if name.startswith("IMAQ_"):
            name = name[5:]

        code = "    public static final {type} {name} = {value};" \
            .format(type=type, name=name, value=clean)
        if after_struct:
            define_after_struct.append((name, code))
            return

        print(code, file=self.out)
        defined.add(name)

    def text(self, text):
        print(text, file=self.out)

    def static_const(self, name, ctype, value):
        # strip IMAQ_ prefix
        if name.startswith("IMAQ_"):
            name = name[5:]

        if hasattr(value, "__iter__"):
            code = "    public static final {ctype} {name} = new {ctype}({value});"
            value = ", ".join(value)
        else:
            code = "{name} = {value};"
        print(code.format(name=name, value=value, ctype=ctype),
              file=self.out)
        defined.add(name)

    def enum(self, name, values):
        if self.config_getboolean(name, "exclude", fallback=False):
            return
        if name in opaque_structs:
            return
        valuestrs = []
        need_search = False
        prev_value = -1
        for vname, value, comment in values:
            if vname.endswith("SIZE_GUARD"):
                continue
            if value is None:
                # auto-increment
                value = "%d" % (prev_value + 1)
            value_i = int(value, 0)
            if value_i < 0 or value_i != (prev_value + 1):
                # need to do search instead of index for fromValue()
                need_search = True
            prev_value = value_i

            if vname == "IMAQdxErrorSuccess":
                continue
            if vname.startswith("IMAQdxError"):
                self.errors[vname] = comment
                continue

            if vname.startswith("IMAQ_"):
                vname = vname[5:]
            if vname.startswith("IMAQdx"):
                vname = vname[6:]
            if vname[0] in "0123456789":
                vname = "C" + vname
            valuestrs.append("%s(%s),%s" % (vname, value, " // %s" % comment if comment else ""))
            defined.add(vname)

        if not valuestrs:
            return

        print("""
    public static enum {name} {{
        {values}
        ;
        private final int value;
        private {name}(int value) {{
            this.value = value;
        }}
        public static {name} fromValue(int val) {{""".format(name=name,
                                                             values="\n        ".join(valuestrs)),
              file=self.out)
        if need_search:
            print("""            for ({name} v : values()) {{
                if (v.value == val)
                    return v;
            }}
            return null;""".format(name=name), file=self.out)
        else:
            print("""            try {{
                return values()[val];
            }} catch (ArrayIndexOutOfBoundsException e) {{
                return null;
            }}""".format(), file=self.out)
        print("""        }}
        public int getValue() {{
            return value;
        }}
    }}""".format(), file=self.out)
        defined.add(name)
        enums.add(name)

    def typedef(self, name, typedef, arr):
        if self.config_getboolean(name, "exclude", fallback=False):
            return
        if name in opaque_structs:
            return
        if typedef.startswith("struct"):
            return
        elif typedef.startswith("union"):
            return
        elif (name, arr) not in java_types_map:
            java_types_map[(name, arr)] = c_to_jtype(typedef, arr).copy()
            if arr is None:
                java_types_map[(name, "")] = c_to_jtype(typedef, "").copy()
        defined.add(name)

    def typedef_function(self, name, restype, params):
        if self.config_getboolean(name, "exclude", fallback=False):
            return
        if name in opaque_structs:
            return
        raise NotImplementedError("typedef function not implemented")

    def function(self, name, restype, params):
        if name not in self.library_funcs:
            return
        if name == "IMAQdxEnumerateVideoModes":
            # full custom code
            print("""
    public static class dxEnumerateVideoModesResult {{
        public IMAQdxEnumItem[] videoModeArray;
        public int currentMode;
        private ByteBuffer videoModeArray_buf;
        private dxEnumerateVideoModesResult(ByteBuffer rv_buf, ByteBuffer videoModeArray_buf) {{
            this.videoModeArray_buf = videoModeArray_buf;
            int count = rv_buf.getInt(0);
            videoModeArray = new IMAQdxEnumItem[count];
            for (int i=0, off=0; i<count; i++, off += {struct_sz}) {{
                videoModeArray[i] = new IMAQdxEnumItem(videoModeArray_buf, off);
                videoModeArray[i].read();
            }}
            currentMode = rv_buf.getInt(8);
        }}
    }}

    public static dxEnumerateVideoModesResult IMAQdxEnumerateVideoModes(int id) {{
        ByteBuffer rv_buf = ByteBuffer.allocateDirect(8+8).order(ByteOrder.nativeOrder());
        long rv_addr = getByteBufferAddress(rv_buf);
        _IMAQdxEnumerateVideoModes(id, 0, rv_addr+0, rv_addr+8);
        int count = rv_buf.getInt(0);
        ByteBuffer videoModeArray_buf = ByteBuffer.allocateDirect(count*{struct_sz}).order(ByteOrder.nativeOrder());
        _IMAQdxEnumerateVideoModes(id, getByteBufferAddress(videoModeArray_buf), rv_addr+0, rv_addr+8);
        dxEnumerateVideoModesResult rv = new dxEnumerateVideoModesResult(rv_buf, videoModeArray_buf);
        return rv;
    }}
    private static native void _IMAQdxEnumerateVideoModes(int id, long videoModeArray, long count, long currentMode);""".format(
                struct_sz=self.config_struct.get("IMAQdxEnumItem", "_SIZE_")), file=self.out)
            print("""
JNIEXPORT void JNICALL Java_{package}_{classname}__1IMAQdxEnumerateVideoModes(JNIEnv* env, jclass , jint id, jlong videoModeArray, jlong count, jlong currentMode)
{{
    IMAQdxError rv = IMAQdxEnumerateVideoModes((IMAQdxSession)id, (IMAQdxVideoMode*)videoModeArray, (uInt32*)count, (uInt32*)currentMode);
    if (rv != IMAQdxErrorSuccess) dxthrowJavaException(env, rv);
}}""".format(package=self.package.replace(".", "_"),
             classname=self.classname), file=self.outc)
            return
        elif name == "IMAQdxGetImageData":
            print("""
    public static int IMAQdxGetImageData(int id, ByteBuffer buffer, IMAQdxBufferNumberMode mode, int desiredBufferNumber) {{
        long buffer_addr = getByteBufferAddress(buffer);
        int buffer_size = buffer.capacity();
        return _IMAQdxGetImageData(id, buffer_addr, buffer_size, mode.getValue(), desiredBufferNumber);
    }}
    private static native int _IMAQdxGetImageData(int id, long buffer, int bufferSize, int mode, int desiredBufferNumber);""".format(),
                  file=self.out)
            print("""
JNIEXPORT jint JNICALL Java_{package}_{classname}__1IMAQdxGetImageData(JNIEnv* env, jclass , jint id, jlong buffer, jint bufferSize, jint mode, jint desiredBufferNumber)
{{
    uInt32 actualBufferNumber;
    IMAQdxError rv = IMAQdxGetImageData((IMAQdxSession)id, (void*)buffer, (uInt32)bufferSize, (IMAQdxBufferNumberMode)mode, (uInt32)desiredBufferNumber, &actualBufferNumber);
    if (rv != IMAQdxErrorSuccess) dxthrowJavaException(env, rv);
    return (jint)actualBufferNumber;
}}""".format(package=self.package.replace(".", "_"),
             classname=self.classname), file=self.outc)
        elif name == "imaqReadFile":
            print("""
    public static void imaqReadFile(Image image, String fileName) {{
        ByteBuffer fileName_buf;
        byte[] fileName_bytes;
        try {{
            fileName_bytes = fileName.getBytes("UTF-8");
        }} catch (UnsupportedEncodingException e) {{
            fileName_bytes = new byte[0];
        }}
        fileName_buf = ByteBuffer.allocateDirect(fileName_bytes.length+1);
        putBytes(fileName_buf, fileName_bytes, 0, fileName_bytes.length).put(fileName_bytes.length, (byte)0);
        _imaqReadFile(image.getAddress(), getByteBufferAddress(fileName_buf), 0, 0);
    }}
    private static native void _imaqReadFile(long image, long fileName, long colorTable, long numColors);""".format(),
                  file=self.out)
            print("""
JNIEXPORT void JNICALL Java_{package}_{classname}__1imaqReadFile(JNIEnv* env, jclass , jlong image, jlong fileName, jlong colorTable, jlong numColors)
{{
    int rv = imaqReadFile((Image*)image, (const char*)fileName, (RGBValue*)colorTable, (int*)numColors);
    if (rv == 0) throwJavaException(env);
}}""".format(package=self.package.replace(".", "_"),
             classname=self.classname), file=self.outc)
            return
        if self.config_getboolean(name, "exclude", fallback=False):
            return

        # common return cases
        retpointer = self.config_getboolean(name, "rvdisposed", fallback=False)
        j_funcargs = []
        jn_funcargs = []
        jni_funcargs = [("env", java_types_map["env", None]),
                        ("", java_types_map["cls", None])]
        jn_passedargs = {}

        paramtypes = {}
        jinit = []
        jfini = []
        exceptioncheck = ""
        retc = ""
        jretc = ""

        if restype == "int":
            functype = "STDFUNC"
            rettype = c_to_jtype("void", None)
            exceptioncheck = "if (rv == 0) throwJavaException(env);"
        elif restype == "IMAQdxError":
            functype = "STDFUNC"
            rettype = c_to_jtype("void", None)
            exceptioncheck = "if (rv != IMAQdxErrorSuccess) dxthrowJavaException(env, rv);"
        else:
            if restype[-1] == "*":
                functype = "STDPTRFUNC"
                exceptioncheck = "if (!rv) throwJavaException(env);"
            else:
                functype = "RETFUNC"

            if restype[-1] == "*" and restype[:-1] in opaque_structs:
                # silently strip pointer from native structs
                rettype = c_to_jtype(restype[:-1], None)
            else:
                rettype = c_to_jtype(restype, None)

        # TODO: defaults
        # defaults = dict((y.strip() for y in x.split(':')) for x in
        #        self.config_get(name, "defaults", "").split(',') if x)

        sized_params = dict(tuple(y.strip() for y in x.split(':')) for x in
                            self.config_get(name, "arraysize", "").split(',') if x)
        size_params = set(sized_params.values())

        inparams = [x.strip() for x in self.config_get(name, "inparams", "").split(',') if
                    x.strip()]
        outparams = [x.strip() for x in self.config_get(name, "outparams", "").split(',') if
                     x.strip()]
        nullokparams = [x.strip() for x in self.config_get(name, "nullok", "").split(',') if
                        x.strip()]
        # guess additional output parameters
        for pname, ptype, arr in params:
            if (pname not in inparams
                and pname not in outparams
                and pname not in sized_params
                and ptype
                and not ptype.startswith("const")
                and ptype[:-1] not in opaque_structs
                and ptype[-1] == "*"):
                outparams.append(pname)

        retarraysize = self.config_get(name, "retarraysize", "").strip()
        if retarraysize:
            size_params.add(retarraysize)
            # rettype = c_to_jtype(restype, "")
            if retarraysize not in outparams:
                outparams.append(retarraysize)

        retsize = self.config_get(name, "retsize", "").strip()
        if retsize:
            size_params.add(retsize)
            if retsize not in outparams:
                outparams.append(retsize)

        retowned = not self.config_getboolean(name, "retunowned", False)

        # Input and output parameter code is generated with the help of
        # "virtual" structures.  The input parameters are collected in one
        # structure, the output parameters in another.  Only if there are
        # multiple output parameters or an output array is the output
        # structure actually generated into the Java code.

        instruct_fields = [(pname, ptype, arr, "") for (pname, ptype, arr)
                           in params if pname != "void"]
        helper = JavaStructEmitHelper(self, name, instruct_fields)
        helper.config_struct_get = lambda o: "0"

        # "pointer" type to use for JN/JNI
        jtype_ptr = java_types_map[("long", None)]

        jfielddefs_private = []
        for fname, ftype, arr, comment in helper.fields:
            # print(fname, ftype, arr)
            is_outparam = (fname in outparams)
            is_nullok = (fname in nullokparams) and not is_outparam

            if is_outparam:
                if ftype[-1] == '*':
                    ftype = ftype[:-1]
                elif arr is None:
                    raise ValueError("outparam %s is not a pointer or array", fname)
            field = helper.get_field_java_code(fname, ftype, arr, 0, jfielddefs_private,
                                               backing="%s_buf" % fname)
            paramtypes[fname] = (ftype, arr, field["type"])
            if is_outparam:
                jn_funcargs.append((fname, jtype_ptr))
                jni_funcargs.append((fname, jtype_ptr))
                continue

            write_bufs = field["write_bufs"]
            write = field["write"]
            jtype = field["type"]
            arr = field["arr"]
            is_pointer = field["is_pointer"]
            to_arg = field["to_arg"]

            # input parameter generation
            if fname not in size_params:
                j_funcargs.append((fname, jtype))
            # for JN/JNI, force all pointer/arrays to simple "long" type
            if ftype[-1] == '*' or jtype.string_array:
                jn_funcargs.append((fname, jtype_ptr))
                jni_funcargs.append((fname, jtype_ptr))
            else:
                jn_funcargs.append((fname, jtype))
                jni_funcargs.append((fname, jtype))

            # determine what to pass and how (e.g. jinit code)
            if jtype.jni_sig == "[Ljava/lang/String;":
                raise NotImplementedError("string array")
            elif jtype.jni_sig[0] == '[':
                if arr:
                    raise NotImplementedError("sized array")
                else:
                    jinit.append("{size_jtype} {size_fname} = {fname}.length;".format(
                        size_jtype=field["size_jtype"].j_type, size_fname=field["size_fname"],
                        fname=fname))
                    jinit.extend("%s = null;" % x for x in write_bufs)
                    jinit.extend(write)
                    jn_passedargs[fname] = to_arg
            elif jtype.is_enum:
                jn_passedargs[fname] = to_arg
            elif jtype.is_opaque:
                if is_nullok:
                    to_arg = "%s == null ? 0 : %s" % (fname, to_arg)
                jn_passedargs[fname] = to_arg
            elif jtype.is_struct:
                if is_nullok:
                    to_arg = "%s == null ? 0 : %s" % (fname, to_arg)
                jn_passedargs[fname] = to_arg
            elif jtype.jni_sig[0] in java_accessor_map:
                jn_passedargs[fname] = fname
            elif jtype.jni_sig == "Ljava/lang/String;":
                if jtype.string_array:
                    jinit.append(
                        "ByteBuffer {fname}_buf = ByteBuffer.allocateDirect({array_size}).order(ByteOrder.nativeOrder());".format(
                            fname=fname, array_size=256))
                    jinit.extend(field["backing_write"])
                    jn_passedargs[
                        fname] = "{fname} == null ? 0 : getByteBufferAddress({fname}_buf)".format(
                        fname=fname)
                else:
                    jinit.extend("%s = null;" % x for x in write_bufs)
                    jinit.extend(write)
                    jn_passedargs[fname] = to_arg
            else:
                raise ValueError("unrecognized jni signature '%s'" % jtype.jni_sig)

        jrettype = rettype.j_type

        outstruct_name = None
        # print(name, jrettype, outparams, retarraysize, retsize)
        if outparams or retarraysize or retsize:
            # create a return structure
            outstruct_fields = []
            outstruct_size = []
            for (pname, ptype, arr) in params:
                if pname not in outparams:
                    continue
                if ptype[-1] == '*':
                    ptype = ptype[:-1]
                outstruct_fields.append((pname, ptype, arr, ""))
                if arr:
                    if ptype == "char":
                        outstruct_size.append(arr)
                    else:
                        raise NotImplementedError("non-char array")
                else:
                    outstruct_size.append("8")
            outstruct_sized_members = {}
            outstruct_name = name[4:] + "Result"

            # create a return buffer (TODO: optimize size)
            jinit.append(
                "ByteBuffer rv_buf = ByteBuffer.allocateDirect(%s).order(ByteOrder.nativeOrder());" % "+".join(
                    outstruct_size))
            jinit.append("long rv_addr = getByteBufferAddress(rv_buf);")

            jconstruct_args = [("rv_buf", "ByteBuffer")]
            jconstruct = []
            if retarraysize:
                jconstruct_args.append(("jn_rv", "long"))
                jconstruct.append("array_addr = jn_rv;")

            if retarraysize:
                outstruct_fields.append(("array", restype, "", ""))
                outstruct_sized_members["array"] = retarraysize
            elif functype != "STDFUNC":
                outstruct_fields.append(("val", restype, None, ""))

            helper = JavaStructEmitHelper(self, outstruct_name, outstruct_fields,
                                          sized_members=outstruct_sized_members)
            helper.config_struct_get = lambda o: "0"
            jfielddefs = []
            jfielddefs_private = []
            off = 0
            for fname, ftype, arr, comment in helper.fields:
                field = helper.get_field_java_code(fname, ftype, arr, off, jfielddefs_private,
                                                   backing="rv_buf")
                if fname == retarraysize:
                    jconstruct.append(
                        field["fielddef"].replace("public ", "").replace(fname, "array_%s" % fname))
                    jconstruct.extend(
                        x.replace(fname, "array_%s" % fname) for x in field["backing_read"])
                    jn_passedargs[fname] = "rv_addr+%d" % off
                    off += 8
                    continue
                jfielddefs.append(field["fielddef"])

                if fname == "array":
                    # jconstruct.extend(field["backing_read"])
                    jconstruct.extend(field["read"])
                elif fname != "val":
                    jn_passedargs[fname] = "rv_addr+%d" % off
                    off += 8
                    jconstruct.extend(field["construct"])
                    jconstruct.extend(field["backing_read"])
                    jconstruct.extend(field["read"])

            if retarraysize:
                jfielddefs.append("private long array_addr;")

            # optimize len(outparams) == 1 case to directly return it.
            if len(outparams) == 1 and not retarraysize and rettype.j_type == "void":
                jfini.extend(x.replace("public ", "") for x in jfielddefs)
                jfini.extend(jconstruct)
                jretc = "return %s;" % outparams[0]
                jrettype = paramtypes[outparams[0]][2].j_type
                # rettype = paramtypes[outparams[0]][2]
            elif len(outparams) == 1 and retsize:
                jfini.extend(x.replace("public ", "") for x in jfielddefs)
                jfini.extend(jconstruct)
                jfini.append("val = new {type}(jn_rv, {owned}, {size});".format(type=rettype.j_type,
                                                                                owned="true" if retowned else "false",
                                                                                size=retsize))
                jretc = "return val;"
            else:
                defined.add(outstruct_name)
                jfini.append("{struct_name} rv = new {struct_name}({args});".format(
                    struct_name=outstruct_name, args=", ".join(x[0] for x in jconstruct_args)))
                if retsize:
                    jfini.append("rv.val = new {type}(jn_rv, {owned}, rv.{size});".format(
                        type=rettype.j_type, owned="true" if retowned else "false", size=retsize))
                elif not retarraysize and functype != "STDFUNC":
                    jfini.append("rv.val = new {type}(jn_rv, {owned});".format(type=rettype.j_type,
                                                                               owned="true" if retowned else "false"))

                jrettype = outstruct_name
                if retarraysize:
                    rettype = c_to_jtype(outstruct_name, None)
                # else:
                #    rettype = java_types_map[("void", None)]

                print("""
    public static class {struct_name} {{
        {jfielddefs}
        private {struct_name}({jconstruct_args}) {{
            {jconstruct}
        }}""".format(struct_name=outstruct_name,
                     jfielddefs="\n        ".join(jfielddefs),
                     jconstruct_args=", ".join("%s %s" % (x[1], x[0]) for x in jconstruct_args),
                     jconstruct="\n            ".join(jconstruct)),
                      file=self.out)
                if retarraysize:
                    print("""
        @Override
        protected void finalize() throws Throwable {{
            imaqDispose(array_addr);
            super.finalize();
        }}
    }}""".format(struct_name=outstruct_name), file=self.out)
                else:
                    print("    }", file=self.out)

                jretc = "return rv;"
        elif functype != "STDFUNC":
            if rettype.is_enum:
                jretc = "return {type}.fromValue(jn_rv);".format(type=rettype.j_type)
            elif rettype.is_struct or rettype.is_opaque:
                jretc = "return new {type}(jn_rv, {owned});".format(type=rettype.j_type,
                                                                    owned="true" if retowned else "false")
            else:
                jretc = "return jn_rv;"

        #
        # Java function
        #
        # assert name.startswith("imaq")
        # name = name[4].lower() + name[5:]

        print("""
    public static {rettype} {name}({args}) {{
        {init}
        {rv}_{name}({passedargs});
        {fini}{retcode}
    }}""".format(rettype=jrettype,
                 name=name,
                 args=", ".join("%s %s" % (x[1].j_type, x[0]) for x in j_funcargs),
                 init="\n        ".join(jinit),
                 fini="\n        ".join(jfini),
                 rv="%s jn_rv = " % rettype.jn_type if rettype.jn_type != "void" else "",
                 retcode="\n        " + jretc if jretc else "",
                 passedargs=", ".join(
                     jn_passedargs[x[0]] if x[0] in jn_passedargs else "UNKNOWN" for x in
                     jn_funcargs)),
              file=self.out)

        #
        # Native Java function
        #
        print("""    private static native {rettype} _{name}({args});""".format(
            rettype=rettype.jn_type, name=name,
            args=", ".join("%s %s" % (x[1].jn_type, x[0]) for x in jn_funcargs)),
            file=self.out)

        #
        # C function
        #
        print("""
/* J: {jrettype} {name}({jargs})
 * JN: {jnrettype} {name}({jnargs})
 * C: {restype} {name}({cparams})
 */""".format(name=name,
              jrettype=rettype.j_type,
              jargs=", ".join("%s %s" % (x[1].j_type, x[0]) for x in j_funcargs),
              jnrettype=rettype.jn_type,
              jnargs=", ".join("%s %s" % (x[1].jn_type, x[0]) for x in jn_funcargs),
              restype=restype,
              cparams=", ".join("%s %s" % (ptype, pname) for pname, ptype, arr in params)),
              file=self.outc)

        cargs = []
        for pname, ptype, arr in params:
            if pname == "void":
                continue
            if ptype in structs:
                cargs.append("*((%s*)%s)" % (ptype, pname))
            elif ptype.endswith("String255"):
                cargs.append("(char *)%s" % pname)
            elif arr:
                cargs.append("(%s*)%s" % (ptype, pname))
            else:
                cargs.append("(%s)%s" % (ptype, pname))

        callcfunc = "{restype} rv = {name}({args});".format(name=name,
                                                            restype=restype,
                                                            args=", ".join(cargs))

        print("""
JNIEXPORT {rettype} JNICALL Java_{package}_{classname}__1{name}({args})
{{
    {callfunc}
    {exceptioncheck}{retcode}
}}""".format(rettype=rettype.jni_type,
             package=self.package.replace(".", "_"),
             classname=self.classname,
             name=name.replace("_", "_1"),
             args=", ".join("%s %s" % (x[1].jni_type, x[0]) for x in jni_funcargs),
             callfunc=callcfunc,
             exceptioncheck=exceptioncheck,
             retcode="\n    return (%s)rv;" % rettype.jni_type if rettype.jni_type != "void" else ""),
              file=self.outc)

        defined.add(name)

    def struct(self, name, fields):
        if self.config_getboolean(name, "exclude", fallback=False):
            return
        if name in opaque_structs:
            return
        defined.add(name)

        helper = JavaStructEmitHelper(self, name, fields)
        print(helper.get_java_code(), file=self.out)

    def union(self, name, fields):
        self.unions[name] = fields


def generate(srcdir, outdir, inputs):
    emit = None

    for fname, config_struct_path, configpath, funcs_path in inputs:
        # read config files
        config_struct = configparser.ConfigParser()
        config_struct.read(config_struct_path)
        config = configparser.ConfigParser()
        config.read(configpath)
        block_comment_exclude = set(x.strip() for x in
                                    config.get("Block Comment", "exclude").splitlines())

        library_funcs = set()
        with open(funcs_path) as ff:
            for line in ff:
                library_funcs.add(line.strip())

        # open input file
        with codecs.open(fname, encoding="utf-8", errors="ignore") as inf:
            # prescan for undefined structures
            prescan_file(inf)
            inf.seek(0)

            if emit is None:
                emit = JavaEmitter(outdir, config, config_struct, library_funcs)
            else:
                emit.config = config
                emit.config_struct = config_struct
                emit.library_funcs = library_funcs

            # generate
            parse_file(emit, inf, block_comment_exclude)

    emit.finish()


if __name__ == "__main__":
    if len(sys.argv) < 5 or ((len(sys.argv) - 1) % 4) != 0:
        print("Usage: gen_wrap.py <header.h config_struct.ini config.ini funcs.txt>...")
        exit(0)

    inputs = []
    for i in range(1, len(sys.argv), 4):
        fname = sys.argv[i]
        config_struct_name = sys.argv[i + 1]
        configname = sys.argv[i + 2]
        funcs_name = sys.argv[i + 3]
        inputs.append((fname, config_struct_name, configname, funcs_name))

    generate("", "", inputs)
