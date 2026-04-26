#!/usr/bin/env python3

import json
import os
from pathlib import Path

TAGS = {
    # --- Categories ---
    # Onboard image processing
    "Vision",
    "AprilTags",
    # Command-based
    "Commandv2",
    "Commandv3",
    # Romi
    "Romi",
    # XRP
    "XRP",
    # Extremely simple programs showcasing a single hardware API
    "Hardware",
    # Full robot, with multiple mechanisms
    "Complete Robot",
    # A single mechanism in the Robot class
    "Basic Robot",
    # --- Mechanisms ---
    "Intake",
    "Flywheel",
    "Elevator",
    "Arm",
    "Differential Drive",
    "Mecanum Drive",
    "Swerve Drive",
    # --- Telemetry ---
    "SmartDashboard",
    "Shuffleboard",
    "Sendable",
    "DataLog",
    # --- Controls ---
    "Exponential Profile",
    "PID",
    "State-Space",
    "LTVUnicycleController",
    "Path Following",
    "Trajectory",
    "SysId",
    "Simulation",
    "Trapezoid Profile",
    "Profiled PID",
    "Odometry",
    "LQR",
    "Pose Estimator",
    # --- Hardware ---
    "Analog",
    "Gyro",
    "Pneumatics",
    "I2C",
    "Duty Cycle",
    "PDP",
    "AddressableLEDs",
    "HAL",
    "Encoder",
    "Smart Motor Controller",
    "Digital Input",
    "Digital Output",
    "Accelerometer",
    "IMU",
    # --- HID ---
    "Gamepad",
    "Joystick",
    # --- RobotBase ---
    "Timed",
    "Timeslice",
    "RobotBase",
    "Educational",
    "OpMode",
    # --- Misc ---
    # (try to keep this section minimal)
    "EventLoop",
    "Mechanism2d",
    "Preferences",
    "Skeleton",
    "FTC",
    "ExpansionHub",
    "OpModes",
}


def parse_bool(value: str | None, default: bool = False) -> bool:
    if value is None:
        return default
    return value.lower() in {"1", "true", "yes"}


def require_key(entry: dict, key: str, index: int, expected_type: type) -> None:
    value = entry.get(key)
    if value is None:
        raise ValueError(f"Entry {index}: missing required field '{key}'")
    if not isinstance(value, expected_type):
        raise TypeError(f"Entry {index}: '{key}' is not {expected_type.__name__}")


def validate_tags(entry: dict, index: int, enforce_allowlist: bool) -> None:
    require_key(entry, "tags", index, list)
    tags = entry["tags"]
    if enforce_allowlist:
        invalid = [tag for tag in tags if tag not in TAGS]
        if invalid:
            raise ValueError(f"Entry {index}: unknown tags: {invalid}")


def file_check(parsed_json: list[dict], folder: Path) -> None:
    folder_names: set[str] = {entry["foldername"] for entry in parsed_json}
    folders = {child.name for child in folder.iterdir() if child.is_dir()}

    disjunct = folders.symmetric_difference(folder_names)
    missing_from_folders = sorted(folder_names.intersection(disjunct))
    missing_from_json = sorted(folders.intersection(disjunct))

    if missing_from_folders or missing_from_json:
        lines = ["Missing From Folders"]
        lines.extend(missing_from_folders)
        lines.append("")
        lines.append("Missing from JSON")
        lines.extend(missing_from_json)
        raise ValueError("Found missing items\n" + "\n".join(lines))


def validate_entries(parsed_json: list[dict], kind: str, is_cpp_commands: bool) -> None:
    for idx, entry in enumerate(parsed_json):
        if not isinstance(entry, dict):
            raise TypeError(f"Entry {idx}: expected object, got {type(entry).__name__}")

        require_key(entry, "name", idx, str)
        require_key(entry, "description", idx, str)
        require_key(entry, "foldername", idx, str)
        match kind:
            case "templates" | "examples":
                validate_tags(entry, idx, enforce_allowlist=True)
                require_key(entry, "gradlebase", idx, str)
                require_key(entry, "commandversion", idx, int)
                if entry["gradlebase"] == "java":
                    require_key(entry, "robotclass", idx, str)
            case "snippets":
                validate_tags(entry, idx, enforce_allowlist=True)
                require_key(entry, "gradlebase", idx, str)
                if entry["gradlebase"] == "java":
                    require_key(entry, "robotclass", idx, str)
            case "commands":
                validate_tags(entry, idx, enforce_allowlist=False)
                require_key(entry, "replacename", idx, str)
                require_key(entry, "commandversion", idx, int)
                if is_cpp_commands:
                    require_key(entry, "headers", idx, list)
                    if not entry["headers"]:
                        raise ValueError(f"Entry {idx}: 'headers' cannot be empty")
                    require_key(entry, "source", idx, list)
                    if not entry["source"]:
                        raise ValueError(f"Entry {idx}: 'source' cannot be empty")
            case _:
                raise ValueError(
                    f"Unknown EXAMPLECHECK_KIND '{kind}'. "
                    "Expected one of: templates, examples, snippets, commands."
                )


def main() -> None:
    kind = os.environ["EXAMPLECHECK_KIND"]
    folder_path = Path(os.environ["EXAMPLECHECK_FOLDER"])
    json_path = folder_path / f"{kind}.json"

    is_cpp_commands = parse_bool(
        os.environ.get("EXAMPLECHECK_IS_CPP_COMMANDS"), default=False
    )

    with json_path.open("rb") as f:
        parsed_json = json.load(f)

    if not isinstance(parsed_json, list):
        raise ValueError(f"Expected top-level JSON array in {json_path}")

    validate_entries(parsed_json, kind, is_cpp_commands)
    file_check(parsed_json, folder_path)


if __name__ == "__main__":
    main()
