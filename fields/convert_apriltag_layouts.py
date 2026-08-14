#!/usr/bin/env python3

"""
This script converts all AprilTag field layout CSV files in
src/generate/resources/org/wpilib/fields/apriltag to tag lists in the matching
field JSON files consumed by the fields pregenerator.

The input CSV has the following format:

* Columns: ID, X, Y, Z, Z Rotation, Y Rotation
* ID is a positive integer
* X, Y, and Z are decimal inches
* Z Rotation is yaw in degrees
* Y Rotation is pitch in degrees

The values come from a table in the layout marking diagram (e.g.,
https://firstfrc.blob.core.windows.net/frc2024/FieldAssets/2024LayoutMarkingDiagram.pdf).
"""

import csv
import json
import os
from pathlib import Path

from wpimath import geometry, units


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

                # Turn yaw into quaternion
                q = geometry.Rotation3d(
                    units.radians(0),
                    units.degreesToRadians(yRotation),
                    units.degreesToRadians(zRotation),
                ).getQuaternion()

                tags.append(
                    {
                        "ID": id,
                        "pose": {
                            "translation": {
                                "x": units.inchesToMeters(x),
                                "y": units.inchesToMeters(y),
                                "z": units.inchesToMeters(z),
                            },
                            "rotation": {
                                "quaternion": {
                                    "W": q.W(),
                                    "X": q.X(),
                                    "Y": q.Y(),
                                    "Z": q.Z(),
                                }
                            },
                        },
                    }
                )

        # Merge the generated field tags into the matching field JSON.
        output_file = output_dir / f"{filename.stem}.json"
        with output_file.open(encoding="utf-8") as f:
            json_data = json.load(f)
        json_data["field-tags"] = tags
        with output_file.open("w", encoding="utf-8") as f:
            json.dump(json_data, f, indent=2)
            f.write("\n")


if __name__ == "__main__":
    main()
