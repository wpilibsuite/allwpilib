#!/usr/bin/env python3

"""
This script converts all AprilTag field layout CSV files in
src/main/native/resources/edu/wpi/first/apriltag to the JSON format
AprilTagFields expects.

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

from wpimath import geometry, units


def main():
    # Find AprilTag field layout CSVs
    filenames = [
        os.path.join(dp, f)
        for dp, dn, fn in os.walk("src/main/native/resources/edu/wpi/first/apriltag")
        for f in fn
        if f.endswith(".csv")
    ]

    for filename in filenames:
        json_data = {"tags": [], "field": {"length": 0.0, "width": 0.0}}

        # Read CSV and fill in JSON data
        with open(filename, newline="") as csvfile:
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

                json_data["tags"].append(
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

        # Write JSON
        with open(filename.replace(".csv", ".json"), "w") as f:
            json.dump(json_data, f, indent=2)
            f.write("\n")


if __name__ == "__main__":
    main()
