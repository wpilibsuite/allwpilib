#!/usr/bin/env python3
#
# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.
#

from pathlib import Path


def check_file_content(file_path):
    with open(file_path, "r") as file:
        lines = file.readlines()

        if file.name.endswith("robot.py"):
            expected_lines = [
                "#!/usr/bin/env python3\n",
                "#\n",
                "# Copyright (c) FIRST and other WPILib contributors.\n",
                "# Open Source Software; you can modify and/or share it under the terms of\n",
                "# the WPILib BSD license file in the root directory of this project.\n",
                "#\n",
                "\n",
            ]
        else:
            expected_lines = [
                "#\n",
                "# Copyright (c) FIRST and other WPILib contributors.\n",
                "# Open Source Software; you can modify and/or share it under the terms of\n",
                "# the WPILib BSD license file in the root directory of this project.\n",
                "#\n",
                "\n",
            ]

        if lines[: len(expected_lines)] != expected_lines:
            print(
                "\n".join(
                    [
                        f"{file_path}",
                        "ERROR: File must start with the following lines",
                        "------------------------------",
                        "".join(expected_lines[:-1]),
                        "------------------------------",
                        "Found:",
                        "".join(lines[: len(expected_lines)]),
                        "------------------------------",
                    ]
                )
            )

            return False
    return True


def main():
    current_directory = Path(__file__).parent
    python_files = [
        x
        for x in current_directory.glob("./**/*.py")
        if not x.parent == current_directory and x.stat().st_size != 0
    ]

    non_compliant_files = [
        file for file in python_files if not check_file_content(file)
    ]

    non_compliant_files.sort()

    if non_compliant_files:
        print("Non-compliant files:")
        for file in non_compliant_files:
            print(f"- {file}")
        exit(1)  # Exit with an error code
    else:
        print("All files are compliant.")


if __name__ == "__main__":
    main()
