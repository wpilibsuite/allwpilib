#!/usr/bin/env python3
#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

import argparse
import datetime

from wpiutil.log import DataLogReader

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("infile")
    args = parser.parse_args()

    reader = DataLogReader(args.infile)

    entries = {}
    for record in reader:
        timestamp = record.getTimestamp() / 1000000
        if record.isStart():
            try:
                data = record.getStartData()
                print(f"{data} [{timestamp}]")
                if data.entry in entries:
                    print("...DUPLICATE entry ID, overriding")
                entries[data.entry] = data
            except TypeError as e:
                print("Start(INVALID)")
        elif record.isFinish():
            try:
                entry = record.getFinishEntry()
                print(f"Finish({entry}) [{timestamp}]")
                if entry not in entries:
                    print("...ID not found")
                else:
                    del entries[entry]
            except TypeError as e:
                print("Finish(INVALID)")
        elif record.isSetMetadata():
            try:
                data = record.getSetMetadataData()
                print(f"{data} [{timestamp}]")
                if data.entry not in entries:
                    print("...ID not found")
            except TypeError as e:
                print("SetMetadata(INVALID)")
        elif record.isControl():
            print("Unrecognized control record")
        else:
            print(f"Data({record.getEntry()}, size={record.getSize()}) ", end="")
            entry = entries.get(record.getEntry(), None)
            if entry is None:
                print("<ID not found>")
                continue
            print(f"<name='{entry.name}', type='{entry.type}'> [{timestamp}]")

            try:
                # handle systemTime specially
                if entry.name == "systemTime" and entry.type == "int64":
                    dt = datetime.fromtimestamp(record.getInteger() / 1000000)
                    print("  {:%Y-%m-%d %H:%M:%S.%f}".format(dt))
                    continue

                if entry.type == "double":
                    print(f"  {record.getDouble()}")
                elif entry.type == "int64":
                    print(f"  {record.getInteger()}")
                elif entry.type == "string" or entry.type == "json":
                    print(f"  '{record.getString()}'")
                elif entry.type == "boolean":
                    print(f"  {record.getBoolean()}")
                elif entry.type == "boolean[]":
                    arr = record.getBooleanArray()
                    print(f"  {arr}")
                elif entry.type == "double[]":
                    arr = record.getDoubleArray()
                    print(f"  {arr}")
                elif entry.type == "float[]":
                    arr = record.getFloatArray()
                    print(f"  {arr}")
                elif entry.type == "int64[]":
                    arr = record.getIntegerArray()
                    print(f"  {arr}")
                elif entry.type == "string[]":
                    arr = record.getStringArray()
                    print(f"  {arr}")
                elif entry.type == "raw":
                    print(f"  {record.getRaw()}")
            except TypeError as e:
                print("  invalid", e)
