# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

"""Bounded tool execution with per-process resource and failure diagnostics."""

import json
import os
import signal
import subprocess
import sys
import time
from contextlib import contextmanager
from datetime import datetime, timezone
from pathlib import Path


def memory_events():
    """Read this process's cgroup v2 counters; absence is not an analysis error."""
    try:
        group = next(
            line[3:]
            for line in Path("/proc/self/cgroup").read_text().splitlines()
            if line.startswith("0::")
        )
        # A container can expose its own cgroup as the mount root.
        for root in (
            Path("/sys/fs/cgroup") / group.lstrip("/"),
            Path("/sys/fs/cgroup"),
        ):
            path = root / "memory.events"
            if path.is_file():
                return {
                    "path": str(path),
                    "counts": {
                        key: int(value)
                        for key, value in (
                            line.split() for line in path.read_text().splitlines()
                        )
                    },
                }
    except (OSError, ValueError, StopIteration):
        pass
    return None


def wait_with_usage(process, timeout):
    """Reap exactly this child, retaining wait4's per-process peak RSS on Unix."""
    if not hasattr(os, "wait4"):
        return process.wait(timeout=timeout), None
    deadline = time.monotonic() + timeout
    while True:
        pid, status, usage = os.wait4(process.pid, os.WNOHANG)
        if pid:
            process.returncode = os.waitstatus_to_exitcode(status)
            return process.returncode, usage
        if time.monotonic() >= deadline:
            raise subprocess.TimeoutExpired(process.args, timeout)
        time.sleep(0.05)


def run_tool(command, log, timeout=1200):
    """Keep tool output, status, timing, memory and timeout evidence in artifacts."""
    command = [str(value) for value in command]
    started = time.monotonic()
    before = memory_events()
    data = {
        "command": command,
        "started_at": datetime.now(timezone.utc).isoformat(),
        "timeout_seconds": timeout,
        "timed_out": False,
        "returncode": None,
    }
    usage = None
    print(f"Tool start: {log.name}: {command[0]}", flush=True)
    with log.open("w", encoding="utf-8") as stream:
        try:
            with subprocess.Popen(
                command,
                stdout=stream,
                stderr=subprocess.STDOUT,
                start_new_session=os.name == "posix",
            ) as process:
                try:
                    data["returncode"], usage = wait_with_usage(process, timeout)
                except subprocess.TimeoutExpired:
                    data["timed_out"] = True
                    if os.name == "posix":
                        os.killpg(process.pid, signal.SIGKILL)
                    else:
                        process.kill()
                    data["returncode"], usage = wait_with_usage(process, 30)
        except OSError as error:
            data["error"] = str(error)
        data["elapsed_seconds"] = round(time.monotonic() - started, 3)
        if usage is not None:
            data.update(
                user_cpu_seconds=round(usage.ru_utime, 3),
                system_cpu_seconds=round(usage.ru_stime, 3),
                # macOS reports bytes; Linux reports KiB. This is a process peak,
                # not the sum of simultaneous memory use by a process tree.
                peak_rss_bytes=int(
                    usage.ru_maxrss * (1 if sys.platform == "darwin" else 1024)
                ),
            )
        code = data["returncode"]
        if code is not None and code < 0 and os.name == "posix":
            data["signal"] = signal.Signals(-code).name
        after = memory_events()
        if before and after and before["path"] == after["path"]:
            data["cgroup_memory_events_delta"] = {
                key: value - before["counts"].get(key, 0)
                for key, value in after["counts"].items()
            }
        diagnostic = log.with_suffix(".process.json")
        diagnostic.write_text(json.dumps(data, indent=2) + "\n", encoding="utf-8")
        status = data.get("signal", f"exit {code}")
        if data["timed_out"]:
            status += f" (timeout after {timeout}s)"
        if data.get("error"):
            status += f": {data['error']}"
        detail = f"{data['elapsed_seconds']:.3f}s, {status}"
        if "peak_rss_bytes" in data:
            detail += f", peak RSS {data['peak_rss_bytes'] / 1024**2:.1f} MiB"
        if data.get("cgroup_memory_events_delta", {}).get("oom_kill", 0):
            detail += (
                ", cgroup OOM kill counter increased (may include other processes)"
            )
        # A killed tool may produce no output at all. Preserve that failure in
        # the text report as well as the structured diagnostics.
        stream.write(f"\nProcess diagnostics: {detail}; see {diagnostic.name}\n")
        print(f"Tool finish: {log.name}: {detail}", flush=True)
    return -1 if data["timed_out"] or code is None else code


@contextmanager
def timed(label, timings):
    """Report progress immediately and retain duration even if a stage fails."""
    started = time.monotonic()
    print(f"Start: {label}", flush=True)
    try:
        yield
    finally:
        elapsed = round(time.monotonic() - started, 3)
        timings[label] = elapsed
        print(f"Finish: {label}: {elapsed:.3f}s", flush=True)
