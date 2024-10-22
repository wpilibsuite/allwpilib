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
    subprocess.run(["python", f"{REPO_ROOT}/hal/generate_usage_reporting.py"])
    subprocess.run(["python", f"{REPO_ROOT}/ntcore/generate_topics.py"])
    subprocess.run(["python", f"{REPO_ROOT}/wpical/generate_mrcal.py"])
    subprocess.run(["python", f"{REPO_ROOT}/wpimath/generate_numbers.py"])
    subprocess.run(
        [
            "python",
            f"{REPO_ROOT}/wpimath/generate_quickbuf.py",
            f"--quickbuf_plugin={args.quickbuf_plugin}",
        ]
    )
    subprocess.run(["python", f"{REPO_ROOT}/wpiunits/generate_units.py"])
    subprocess.run(["python", f"{REPO_ROOT}/wpilibc/generate_hids.py"])
    subprocess.run(["python", f"{REPO_ROOT}/wpilibj/generate_hids.py"])
    subprocess.run(["python", f"{REPO_ROOT}/wpilibNewCommands/generate_hids.py"])
    subprocess.run(["python", f"{REPO_ROOT}/wpilibc/generate_pwm_motor_controllers.py"])
    subprocess.run(["python", f"{REPO_ROOT}/wpilibj/generate_pwm_motor_controllers.py"])
    subprocess.run(["python", f"{REPO_ROOT}/thirdparty/imgui_suite/generate_gl3w.py"])
    subprocess.run(f"{REPO_ROOT}/thirdparty/imgui_suite/generate_fonts.sh")


if __name__ == "__main__":
    main(sys.argv[1:])
