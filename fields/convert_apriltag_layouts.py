#!/usr/bin/env python3

"""
This script converts all AprilTag field layout CSV files in
src/generate/resources/org/wpilib/fields/apriltag to tag lists in the matching
field JSON files consumed by the fields pregenerator.

The input CSV has the following format:

* Columns: ID, X, Y, Z, Z-Rotation, Y-Rotation
* ID is a positive integer
* X, Y, and Z are decimal inches
* Z Rotation is yaw in degrees
* Y Rotation is pitch in degrees
* The origin is at field center on the floor, with +X toward blue and +Z up
* +Y points left when facing +X, and yaw zero faces +X

Input and output poses use the same field coordinate system. Official CSVs
from 2027 onward use this convention; the checked-in earlier CSVs have already
been converted. This script only converts inches to meters and Euler angles
to quaternions.

The values come from a table in the layout marking diagram (e.g.,
https://firstfrc.blob.core.windows.net/frc2024/FieldAssets/2024LayoutMarkingDiagram.pdf).
"""

import csv
import json
import math
import os
from pathlib import Path

INCH_TO_METER = 0.0254


def main():
    script_dir = Path(__file__).resolve().parent
    workspace_dir = Path(os.environ.get("BUILD_WORKSPACE_DIRECTORY", script_dir))
    project_dir = (
        workspace_dir / "fields" if (workspace_dir / "fields").exists() else script_dir
    )
    input_dir = project_dir / "src/generate/resources/org/wpilib/fields/apriltag"
    output_dir = project_dir / "src/main/native/resources/org/wpilib/fields/frc"

    # Find AprilTag field layout CSVs
    filenames = sorted(input_dir.glob("*.csv"))

    for filename in filenames:
        output_file = output_dir / f"{filename.stem}.json"
        with output_file.open(encoding="utf-8") as f:
            json_data = json.load(f)
        tags = []

        # Read CSV and fill in JSON data
        with filename.open(newline="") as csvfile:
            reader = csv.reader(csvfile, delimiter=",")

            # Skip header
            next(reader)

            for row in reader:
                # Unpack row elements
                id = int(row[0])
                x = float(row[1])
                y = float(row[2])
                z = float(row[3])
                zRotation = float(row[4])
                yRotation = float(row[5])

                # The CSV already uses the centered field frame.
                yaw = math.radians(zRotation) / 2
                pitch = math.radians(yRotation) / 2
                w = math.cos(pitch) * math.cos(yaw)
                qx = -math.sin(pitch) * math.sin(yaw)
                qy = math.sin(pitch) * math.cos(yaw)
                qz = math.cos(pitch) * math.sin(yaw)

                tags.append(
                    {
                        "ID": id,
                        "pose": {
                            "translation": {
                                "x": x * INCH_TO_METER,
                                "y": y * INCH_TO_METER,
                                "z": z * INCH_TO_METER,
                            },
                            "rotation": {
                                "quaternion": {
                                    "W": w,
                                    "X": qx,
                                    "Y": qy,
                                    "Z": qz,
                                }
                            },
                        },
                    }
                )

        # Merge the generated field tags into the matching field JSON.
        json_data["field-tags"] = tags
        with output_file.open("w", encoding="utf-8") as f:
            json.dump(json_data, f, indent=2)
            f.write("\n")


if __name__ == "__main__":
    main()
