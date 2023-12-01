#!/usr/bin/env python3

import json
import pathlib

import matplotlib.pyplot as plt
import pandas as pd
import sys

# Load data
filename = pathlib.Path(sys.argv[1])

UNIT_TO_ABBREVIATION = {
    "Meters": "m",
    "Feet": "ft",
    "Inches": "in",
    "Degrees": "deg",
    "Rotations": "rot",
    "Radians": "rad",
}

# Make DataFrame to facilitate plotting
if filename.suffix == ".json":
    raw_data = json.loads(filename.read_text())
    unit = raw_data["units"]

    # Get Unit
    try:
        abbreviation = UNIT_TO_ABBREVIATION[unit]
    except KeyError:
        raise ValueError("Invalid Unit")

    # Make Columns
    columns = ["Timestamp (s)", "Test"]
    if "Drive" in raw_data["test"]:
        columns.extend(
            [
                "Left Volts (V)",
                "Right Volts (V)",
                f"Left Position ({abbreviation})",
                f"Right Position ({abbreviation})",
                f"Left Velocity ({abbreviation}/s)",
                f"Right Velocity ({abbreviation}/s)",
                "Gyro Position (deg)",
                "Gyro Rate (deg/s)",
            ]
        )
        unit_columns = columns[4:8]
    else:
        columns.extend(
            ["Volts (V)", f"Position ({abbreviation})", f"Velocity ({abbreviation}/s)"]
        )
        unit_columns = columns[3:]

    prepared_data = pd.DataFrame(columns=columns)
    for test in raw_data.keys():
        if "-" not in test:
            continue
        formatted_entry = [[pt[0], test, *pt[1:]] for pt in raw_data[test]]
        prepared_data = pd.concat(
            [prepared_data, pd.DataFrame(formatted_entry, columns=columns)]
        )

    units_per_rot = raw_data["unitsPerRotation"]

    for column in unit_columns:
        prepared_data[column] *= units_per_rot
else:
    prepared_data = pd.read_csv(filename)

# First 2 columns are Timestamp and Test
for column in prepared_data.columns[2:]:
    # Configure Plot Labels
    plt.figure()
    plt.xlabel("Timestamp (s)")
    plt.ylabel(column)

    # Configure title without units
    print(column)
    end = column.find("(")
    plt.title(f"{column[:end].strip()} vs Time")

    # Plot data for each test
    for test in pd.unique(prepared_data["Test"]):
        test_data = prepared_data[prepared_data["Test"] == test]
        plt.plot(test_data["Timestamp (s)"], test_data[column], label=test)
    plt.legend()

plt.show()
