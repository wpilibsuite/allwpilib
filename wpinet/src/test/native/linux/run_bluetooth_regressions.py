#!/usr/bin/env python3
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

"""Run isolated Linux Bluetooth socket regressions against a CMake shared build."""

import argparse
import json
import os
from pathlib import Path
import shlex
import subprocess
import sys
import tempfile


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "build_dir",
        type=Path,
        help="CMake build with compile_commands.json and shared wpinet",
    )
    args = parser.parse_args()
    if sys.platform != "linux":
        parser.error("these socket regressions require Linux")
    build = args.build_dir.resolve()
    database = build / "compile_commands.json"
    if not database.is_file():
        parser.error("configure CMake with -DCMAKE_EXPORT_COMPILE_COMMANDS=ON")
    entries = json.loads(database.read_text())
    entry = next(
        (
            item
            for item in entries
            if item["file"].endswith("/linux/BluetoothLEPacketClient.cpp")
        ),
        None,
    )
    if entry is None:
        parser.error("build directory does not contain the Linux Bluetooth client")
    arguments = entry.get("arguments") or shlex.split(entry["command"])
    includes = []
    i = 1
    while i < len(arguments):
        arg = arguments[i]
        if arg in ("-I", "-isystem", "-iquote", "-D"):
            includes.extend(arguments[i : i + 2])
            i += 2
            continue
        if arg.startswith(("-I", "-D")):
            includes.append(arg)
        i += 1
    libraries = list((build / "lib").glob("libwpinet*.so"))
    if len(libraries) != 1:
        parser.error(
            "expected one built shared lib/libwpinet[debug suffix].so; build wpinet first"
        )
    wpinet = libraries[0]
    wpiutil = wpinet.with_name(wpinet.name.replace("wpinet", "wpiutil", 1))
    if not wpiutil.is_file():
        parser.error(f"missing {wpiutil}; build wpinet first")
    compiler = shlex.split(os.environ.get("CXX", arguments[0]))
    source_dir = Path(__file__).resolve().parent
    suites = {
        "BluetoothPollRegression": ["fallback", "gatt", "cleared", "established"],
        "BluetoothRetryRegression": [
            "fallback",
            "direct",
            "persistent",
            "fatal",
            "cancel",
            "replace",
        ],
    }
    with tempfile.TemporaryDirectory(
        prefix="wpinet-bluetooth-regressions-"
    ) as temporary:
        for suite, scenarios in suites.items():
            executable = str(Path(temporary) / suite)
            command = compiler + [
                "-std=c++23",
                "-g",
                *includes,
                str(source_dir / (suite + ".cpp")),
                str(wpinet),
                str(wpiutil),
                "-Wl,-rpath," + str(build / "lib"),
                "-ldl",
                "-pthread",
                "-o",
                executable,
            ]
            subprocess.run(command, cwd=entry["directory"], check=True)
            # Debug output must not change error handling or emit anything by default.
            for debug in (False, True):
                env = os.environ.copy()
                env.pop("WPI_BLUETOOTH_DEBUG", None)
                if debug:
                    env["WPI_BLUETOOTH_DEBUG"] = "1"
                for scenario in scenarios:
                    result = subprocess.run(
                        [executable, scenario],
                        env=env,
                        capture_output=True,
                        text=True,
                        timeout=8,
                    )
                    if result.returncode or (not debug and result.stderr):
                        sys.stdout.write(result.stdout)
                        sys.stderr.write(result.stderr)
                        raise SystemExit(f"{suite}/{scenario} failed (debug={debug})")
                    if debug and "[BT " not in result.stderr:
                        raise SystemExit(
                            f"{suite}/{scenario}: expected enabled diagnostics"
                        )
                    print(f"{suite}/{scenario}: passed (debug={debug})", flush=True)


if __name__ == "__main__":
    main()
