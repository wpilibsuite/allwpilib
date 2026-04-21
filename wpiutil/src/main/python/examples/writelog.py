#!/usr/bin/env python3

import argparse
import pathlib

from wpiutil.log import DataLog, BooleanLogEntry, StringArrayLogEntry, RawLogEntry


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("out", type=pathlib.Path)
    args = parser.parse_args()

    if args.out.is_dir():
        datalog = DataLog(str(args.out))
    else:
        datalog = DataLog(str(args.out.parent), args.out.name)

    bools = BooleanLogEntry(datalog, "/bools")
    bools.append(True)
    bools.append(False)

    strings = StringArrayLogEntry(datalog, "/strings")
    strings.append(["a", "b", "c"])
    strings.append(["d", "e", "f"])

    raw = RawLogEntry(datalog, "/raws")
    raw.append(b"\x01\x02\x03")
    raw.append(b"\x04\x05\x06")
