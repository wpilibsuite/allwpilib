#!/usr/bin/env python3

# Copyright (c) FIRST and other WPILib contributors.
# Open Source Software; you can modify and/or share it under the terms of
# the WPILib BSD license file in the root directory of this project.

import argparse
import json
import re
from pathlib import Path

from jinja2 import Environment, FileSystemLoader

BASE_RESOURCE_DIR = "/org/wpilib/fields/"
FIELDS_RESOURCE_DIR = "org/wpilib/fields"


def write_file(output_dir: Path, path: str, contents: str):
    output_file = output_dir / path
    output_file.parent.mkdir(parents=True, exist_ok=True)
    output_file.write_text(contents, encoding="utf-8", newline="\n")


def resource_function(filename: str) -> str:
    return "GetResource_" + re.sub(r"[^a-zA-Z0-9]", "_", filename)


def string_literal(value: str) -> str:
    return json.dumps(value)


def java_nullable_string(value: str | None) -> str:
    if value is None:
        return "null"
    return string_literal(value)


def cpp_nullable_string(value: str | None) -> str:
    if value is None:
        return "nullptr"
    return string_literal(value)


def number_literal(value: float) -> str:
    return json.dumps(value)


def cpp_meters(value: float) -> str:
    return f"wpi::units::meters<>{{{number_literal(value)}}}"


def java_identifier_suffix(value: str) -> str:
    parts = re.split(r"[^a-zA-Z0-9]+", value.removesuffix(".json"))
    return "".join(part[:1].upper() + part[1:] for part in parts if part)


def java_method_name(prefix: str, value: str) -> str:
    return prefix + java_identifier_suffix(value)


def default_enum_name(rel_json: str) -> str:
    program, filename = rel_json.removesuffix(".json").split("/", 1)
    name = re.sub(r"[^a-zA-Z0-9]", "_", filename).upper()
    return f"{program.upper()}_{name}"


def validate_number_object(value: dict, key: str, json_file: Path) -> int | float:
    item = value[key]
    if not isinstance(item, int | float) or isinstance(item, bool):
        raise TypeError(f"{json_file}: {key} must be a number")
    return item


def validate_field(json_file: Path, rel_json: str, field: dict):
    for key in (
        "name",
        "season",
        "game",
        "field-dimensions",
        "program",
    ):
        if key not in field:
            raise ValueError(f"{json_file} is missing {key}")

    for key in ("name", "season", "game", "program"):
        if not isinstance(field[key], str):
            raise TypeError(f"{json_file}: {key} must be a string")

    program = rel_json.split("/", 1)[0]
    if field["program"] != program:
        raise ValueError(
            f"{json_file}: program {field['program']} does not match {program}"
        )

    if "field-image" in field:
        image = field["field-image"]
        if not isinstance(image, dict):
            raise ValueError(f"{json_file}: field-image must be an object")
        if not isinstance(image.get("path"), str):
            raise ValueError(f"{json_file}: field-image path must be a string")
        for key in ("top", "left", "bottom", "right"):
            if not isinstance(image.get(key), int) or isinstance(image.get(key), bool):
                raise TypeError(f"{json_file}: field-image {key} must be an integer")

    field_dimensions = field["field-dimensions"]
    if not isinstance(field_dimensions, dict):
        raise TypeError(f"{json_file}: field-dimensions must be an object")
    validate_number_object(field_dimensions, "length", json_file)
    validate_number_object(field_dimensions, "width", json_file)

    if "field-tags" in field:
        validate_field_tags(json_file, field["field-tags"])


def validate_field_tags(json_file: Path, tags: list[dict]):
    if not isinstance(tags, list):
        raise TypeError(f"{json_file}: field-tags must be an array")

    for tag in tags:
        if not isinstance(tag, dict):
            raise TypeError(f"{json_file}: each tag must be an object")
        if not isinstance(tag.get("ID"), int):
            raise TypeError(f"{json_file}: each tag ID must be an integer")
        pose = tag.get("pose")
        if not isinstance(pose, dict):
            raise TypeError(f"{json_file}: each tag pose must be an object")
        translation = pose.get("translation")
        if not isinstance(translation, dict):
            raise TypeError(f"{json_file}: each translation must be an object")
        for key in ("x", "y", "z"):
            validate_number_object(translation, key, json_file)
        rotation = pose.get("rotation")
        if not isinstance(rotation, dict):
            raise TypeError(f"{json_file}: each rotation must be an object")
        quaternion = rotation.get("quaternion")
        if not isinstance(quaternion, dict):
            raise TypeError(f"{json_file}: each quaternion must be an object")
        for key in ("W", "X", "Y", "Z"):
            validate_number_object(quaternion, key, json_file)


def validate_generator_config(config_file: Path, config: dict):
    if not isinstance(config.get("defaultField"), str):
        raise TypeError(f"{config_file}: defaultField must be a string")
    if not isinstance(config.get("fields"), list):
        raise TypeError(f"{config_file}: fields must be an array")

    resources = set()
    enums = set()
    for entry in config["fields"]:
        if not isinstance(entry, dict):
            raise TypeError(f"{config_file}: each field entry must be an object")
        for key in ("resource", "enum"):
            if not isinstance(entry.get(key), str):
                raise TypeError(f"{config_file}: each field entry needs {key}")
        if entry["resource"] in resources:
            raise ValueError(f"{config_file}: duplicate resource {entry['resource']}")
        if entry["enum"] in enums:
            raise ValueError(f"{config_file}: duplicate enum {entry['enum']}")
        resources.add(entry["resource"])
        enums.add(entry["enum"])
    if config["defaultField"] not in enums:
        raise ValueError(f"{config_file}: defaultField is not an enum")


def load_generator_config(template_root: Path) -> dict:
    config_file = template_root / "fields.json"
    config = json.loads(config_file.read_text(encoding="utf-8"))
    validate_generator_config(config_file, config)

    field_enum_names = {}
    entry_order = {}
    for i, entry in enumerate(config["fields"]):
        field_enum_names[entry["resource"]] = entry["enum"]
        entry_order[entry["enum"]] = i

    return {
        "default_field": config["defaultField"],
        "field_enum_names": field_enum_names,
        "entry_order": entry_order,
    }


def normalize_tags(tags: list[dict]) -> list[dict]:
    normalized = []
    for tag in tags:
        normalized.append(
            {
                "id": tag["ID"],
                "translation": tag["pose"]["translation"],
                "quaternion": tag["pose"]["rotation"]["quaternion"],
            }
        )
    return normalized


def load_fields(resources_root: Path, generator_config: dict) -> dict[str, dict]:
    base_dir = resources_root / FIELDS_RESOURCE_DIR
    fields = {}
    for program_dir in ("frc", "ftc"):
        for json_file in sorted((base_dir / program_dir).glob("*.json")):
            rel_json = json_file.relative_to(base_dir).as_posix()
            field = json.loads(json_file.read_text(encoding="utf-8"))

            validate_field(json_file, rel_json, field)

            program = field["program"]
            image = field.get("field-image")
            field_image = None
            image_func = None
            top = 0
            left = 0
            bottom = 0
            right = 0
            if image is not None:
                field_image = f"{program}/{image['path']}"
                image_func = resource_function(Path(image["path"]).name)
                top = image["top"]
                left = image["left"]
                bottom = image["bottom"]
                right = image["right"]

            enum_name = generator_config["field_enum_names"].get(
                rel_json, default_enum_name(rel_json)
            )
            has_tags = "field-tags" in field
            layout = None
            if has_tags:
                layout = {
                    "enum": enum_name,
                    "factory_method": java_method_name("create", enum_name.lower())
                    + "Tags",
                }
            java_tags_arg = "null"
            cpp_tags_span = "std::span<const FieldTag>{}"
            if layout is not None:
                java_tags_arg = f"Fields::{layout['factory_method']}"
                cpp_tags_span = f"std::span{{FIELD_TAGS_{layout['enum']}}}"

            dimensions = field["field-dimensions"]
            fields[rel_json] = {
                "json": field,
                "rel_json": rel_json,
                "name": field["name"],
                "season": field["season"],
                "game": field["game"],
                "resource_file": rel_json,
                "image_resource_file": field_image,
                "top": top,
                "left": left,
                "bottom": bottom,
                "right": right,
                "length": dimensions["length"],
                "width": dimensions["width"],
                "program": program,
                "enum": enum_name,
                "layout": layout,
                "has_tags": has_tags,
                "java_tags_arg": java_tags_arg,
                "cpp_has_tags": "true" if has_tags else "false",
                "cpp_tags_span": cpp_tags_span,
                "tags": normalize_tags(field.get("field-tags", [])),
                "order": generator_config["entry_order"].get(
                    enum_name,
                    len(generator_config["entry_order"]) + len(fields),
                ),
                "json_func": resource_function(json_file.name),
                "image_func": image_func,
            }
    return fields


def build_field_entries(fields: dict[str, dict]) -> list[dict]:
    return list(fields.values())


def entry_order_key(field: dict):
    return field["order"], field["rel_json"]


def cpp_display_sort_key(field: dict):
    years = [int(year) for year in re.findall(r"\d{4}", field["rel_json"])]
    latest_year = max(years)
    program_order = 0 if field["program"] == "frc" else 1
    return (-latest_year, program_order, field["order"])


def unique_tag_entries(entries: list[dict], key: str) -> list[dict]:
    result = []
    generated_layouts = set()
    for entry in entries:
        layout = entry["layout"]
        if layout and layout[key] not in generated_layouts:
            result.append(entry)
            generated_layouts.add(layout[key])
    return result


def unique_image_fields(fields: dict[str, dict]) -> list[dict]:
    result = []
    image_resources = set()
    for field in sorted(fields.values(), key=cpp_display_sort_key):
        if field["image_resource_file"] is None:
            continue
        if field["image_resource_file"] in image_resources:
            continue
        result.append(field)
        image_resources.add(field["image_resource_file"])
    return result


def make_environment(template_root: Path) -> Environment:
    env = Environment(
        loader=FileSystemLoader(template_root),
        autoescape=False,
        keep_trailing_newline=True,
        lstrip_blocks=True,
        trim_blocks=True,
    )
    env.filters["string"] = string_literal
    env.filters["java_nullable_string"] = java_nullable_string
    env.filters["cpp_nullable_string"] = cpp_nullable_string
    env.filters["number"] = number_literal
    env.filters["cpp_meters"] = cpp_meters
    return env


def render_template(env: Environment, template: str, context: dict) -> str:
    return env.get_template(template).render(**context)


def generate_fields(output_directory: Path, resources_root: Path, template_root: Path):
    generator_config = load_generator_config(template_root)
    fields = load_fields(resources_root, generator_config)
    entries = build_field_entries(fields)
    entries_by_enum = sorted(entries, key=entry_order_key)

    context = {
        "base_resource_dir": BASE_RESOURCE_DIR,
        "default_field": generator_config["default_field"],
        "entries": entries_by_enum,
        "entries_by_display": sorted(entries, key=cpp_display_sort_key),
        "field_images": unique_image_fields(fields),
        "java_tag_entries": unique_tag_entries(entries_by_enum, "factory_method"),
        "cpp_tag_entries": unique_tag_entries(entries_by_enum, "enum"),
        "json_funcs": sorted({entry["json_func"] for entry in entries}),
    }

    env = make_environment(template_root)
    write_file(
        output_directory,
        "main/java/org/wpilib/fields/Fields.java",
        render_template(env, "main/java/Fields.java.jinja", context),
    )
    write_file(
        output_directory,
        "main/native/cpp/fields/fields.cpp",
        render_template(env, "main/native/cpp/fields/fields.cpp.jinja", context),
    )
    write_file(
        output_directory,
        "main/native/include/wpi/fields/fields.hpp",
        render_template(
            env, "main/native/include/wpi/fields/fields.hpp.jinja", context
        ),
    )
    write_file(
        output_directory,
        "main/native/cpp/fieldimages/field_images.cpp",
        render_template(
            env, "main/native/cpp/fieldimages/field_images.cpp.jinja", context
        ),
    )


def main():
    script_path = Path(__file__).resolve()
    dirname = script_path.parent

    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--output_directory",
        default=dirname / "src/generated",
        type=Path,
        help="Directory to write generated source files into",
    )
    parser.add_argument(
        "--resources_root",
        default=dirname / "src/main/native/resources",
        type=Path,
        help="Root directory containing org/wpilib/fields JSON resources",
    )
    parser.add_argument(
        "--template_root",
        default=dirname / "src/generate",
        type=Path,
        help="Root directory containing the fields generator data and templates",
    )
    args = parser.parse_args()

    generate_fields(args.output_directory, args.resources_root, args.template_root)


if __name__ == "__main__":
    main()
