from __future__ import print_function
import sys
import os
import re
try:
    import configparser
except ImportError:
    import ConfigParser as configparser

from nivision_parse import *

class StructSizerEmitter:
    def __init__(self, out, config):
        self.out = out
        self.config = config
        print("""#include <stdio.h>
#include <nivision.h>

int main()
{
    printf("[_platform_]\\npointer=%d\\n", (int)sizeof(void*));
""", file=self.out)

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

        print('printf("[{name}]\\n_SIZE_=%d\\n", (int)sizeof({name}));'.format(name=name), file=self.out)

        for fname, ftype, arr, comment in fields:
            if ':' in fname:
                continue # can't handle bitfields
            print('printf("{field}=%d\\n", (int)offsetof({name}, {field}));'.format(name=name, field=fname), file=self.out)

    def struct(self, name, fields):
        self.structunion("Structure", name, fields)

    def union(self, name, fields):
        self.structunion("Union", name, fields)

def generate(srcdir, configpath=None, nivisionhpath=None):
    # read config file
    config = configparser.ConfigParser()
    config.read(configpath)
    block_comment_exclude = set(x.strip() for x in
            config.get("Block Comment", "exclude").splitlines())

    # open input file
    inf = open(nivisionhpath)

    # prescan for undefined structures
    prescan_file(inf)
    inf.seek(0)

    # generate
    with open("struct_sizer.c", "wt") as out:
        emit = StructSizerEmitter(out, config)
        parse_file(emit, inf, block_comment_exclude)
        emit.finish()

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: gen_struct_sizer.py <nivision.h> [config.ini]")
        exit(0)

    fname = sys.argv[1]
    configname = "nivision_2011.ini"
    if len(sys.argv) >= 3:
        configname = sys.argv[2]

    generate("", configname, fname)
