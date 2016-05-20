from __future__ import print_function

import os
import sys

try:
    import configparser
except ImportError:
    import ConfigParser as configparser

from nivision_parse import *


class StructSizerEmitter:
    def __init__(self, out, config, hname):
        self.out = out
        self.config = config
        print("""#include <stdio.h>
#include <stddef.h>
#include <{hname}>

int main()
{{
    asm("#STRUCT_SIZER [_platform_]\\n");
    asm("#STRUCT_SIZER pointer=%0\\n" : : "n"((int)sizeof(void*)));
""".format(hname=hname), file=self.out)

    def finish(self):
        print("}", file=self.out)

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
        pass

    def opaque_struct(self, name):
        pass

    def define(self, name, value, comment):
        pass

    def text(self, text):
        print(text, file=self.out)

    def static_const(self, name, ctype, value):
        pass

    def enum(self, name, values):
        pass

    def typedef(self, name, typedef, arr):
        pass

    def typedef_function(self, name, restype, params):
        pass

    def function(self, name, restype, params):
        pass

    def structunion(self, ctype, name, fields):
        if name in opaque_structs:
            return

        print('asm("#STRUCT_SIZER [{name}]\\n");'.format(name=name), file=self.out)
        print('asm("#STRUCT_SIZER _SIZE_=%0\\n" : : "n"((int)sizeof({name})));'.format(name=name),
              file=self.out)

        for fname, ftype, arr, comment in fields:
            if ':' in fname:
                continue  # can't handle bitfields
            print(
                'asm("#STRUCT_SIZER {field}=%0\\n" : : "n"((int)offsetof({name}, {field})));'.format(
                    name=name, field=fname), file=self.out)

    def struct(self, name, fields):
        self.structunion("Structure", name, fields)

    def union(self, name, fields):
        self.structunion("Union", name, fields)


def generate(srcdir, configpath=None, hpath=None):
    # read config file
    config = configparser.ConfigParser()
    config.read(configpath)
    block_comment_exclude = set(x.strip() for x in
                                config.get("Block Comment", "exclude").splitlines())

    # open input file
    inf = open(hpath)

    # prescan for undefined structures
    prescan_file(inf)
    inf.seek(0)

    # generate
    with open("struct_sizer.c", "wt") as out:
        emit = StructSizerEmitter(out, config, os.path.basename(hpath))
        parse_file(emit, inf, block_comment_exclude)
        emit.finish()


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: gen_struct_sizer.py <header.h> <config.ini>")
        exit(0)

    fname = sys.argv[1]
    configname = sys.argv[2]

    generate("", configname, fname)
