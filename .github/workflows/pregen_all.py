#!/usr/bin/env python3

import argparse
import subprocess
import sys
from pathlib import Path


def main(argv):
    script_path = Path(__file__).resolve()
    REPO_ROOT = script_path.parent.parent.parent
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--quickbuf_plugin",
        help="Path to the quickbuf protoc plugin",
        required=True,
    )
    args = parser.parse_args(argv)
    subprocess.run(
        [sys.executable, f"{REPO_ROOT}/hal/generate_usage_reporting.py"], check=True
    )
    subprocess.run(
        [sys.executable, f"{REPO_ROOT}/ntcore/generate_topics.py"], check=True
    )
    subprocess.run(
        [sys.executable, f"{REPO_ROOT}/wpimath/generate_numbers.py"], check=True
    )
    subprocess.run(
        [
            sys.executable,
            f"{REPO_ROOT}/wpimath/generate_quickbuf.py",
            f"--quickbuf_plugin={args.quickbuf_plugin}",
        ],
        check=True,
    )
    subprocess.run(
        [sys.executable, f"{REPO_ROOT}/wpiunits/generate_units.py"], check=True
    )
    subprocess.run(
        [sys.executable, f"{REPO_ROOT}/wpilibc/generate_hids.py"], check=True
    )
    subprocess.run(
        [sys.executable, f"{REPO_ROOT}/wpilibj/generate_hids.py"], check=True
    )
    subprocess.run(
        [sys.executable, f"{REPO_ROOT}/wpilibNewCommands/generate_hids.py"], check=True
    )
    subprocess.run(
        [sys.executable, f"{REPO_ROOT}/wpilibc/generate_pwm_motor_controllers.py"],
        check=True,
    )
    subprocess.run(
        [sys.executable, f"{REPO_ROOT}/wpilibj/generate_pwm_motor_controllers.py"],
        check=True,
    )
    subprocess.run(
        [sys.executable, f"{REPO_ROOT}/thirdparty/imgui_suite/generate_gl3w.py"],
        check=True,
    )
    subprocess.run(f"{REPO_ROOT}/thirdparty/imgui_suite/generate_fonts.sh", check=True)


if __name__ == "__main__":
    main(sys.argv[1:])
