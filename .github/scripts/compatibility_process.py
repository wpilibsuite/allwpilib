# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

"""Bounded tool execution with failure diagnostics."""

import os
import signal
import subprocess


def run_tool(command, log, timeout=1200):
    """Keep tool output and explicit failures in the text artifact."""
    command = [str(value) for value in command]
    with log.open("w", encoding="utf-8") as stream:
        try:
            with subprocess.Popen(
                command,
                stdout=stream,
                stderr=subprocess.STDOUT,
                start_new_session=os.name == "posix",
            ) as process:
                try:
                    code = process.wait(timeout=timeout)
                except subprocess.TimeoutExpired:
                    if os.name == "posix":
                        os.killpg(process.pid, signal.SIGKILL)
                    else:
                        process.kill()
                    process.wait()
                    stream.write(f"\nTool timed out after {timeout}s.\n")
                    return -1
        except OSError as error:
            stream.write(f"\nTool failed to start: {error}\n")
            return -1
        if code < 0 and os.name == "posix":
            stream.write(f"\nTool terminated by {signal.Signals(-code).name}.\n")
        return code
