# fields

The library where FIRST field metadata and optional field images are stored for use by other programs.

## Adding new field images

### Adding the image

Field images, if stored in PNG format, should be compressed with [oxipng](https://github.com/oxipng/oxipng) using `oxipng -o max --fast --strip safe -z fieldImage.png` to ensure the image is as small as possible. They should then be placed in `src/main/native/resources/org/wpilib/fields/<program>`, with the name `YEAR-gamename`, with `<program>` being either `ftc` or `frc`. For FTC, the year is a pair of years, like `2025-2026`.

### Adding the JSON

A JSON file should also be placed in `src/main/native/resources/org/wpilib/fields/<program>`, with required standard fields and optional `field-image` and `field-tags` fields:

- `name`, the full human-readable field name, including the season and program
- `season`, the season year or year range
- `game`, the game name without the season or program
- `field-dimensions`, an object with `length` and `width` fields measured in meters
- `program`, which is either `ftc` or `frc`
- `field-image`, an optional object with `path`, `top`, `left`, `bottom`, and `right` fields. `path` is relative to the directory containing the JSON file, and the other fields specify the boundaries of the field in image pixels.
- `field-tags`, an optional list of field tag metadata for official built-in layouts

For fields with images, X is 0 at the left edge and increases to the right, and Y is 0 at the top edge and increases going down. The field image must be oriented so that the field length runs along the horizontal left/right axis and the field width runs along the vertical top/bottom axis. In other words, `field-dimensions.length` corresponds to the distance between `field-image.left` and `field-image.right`, while `field-dimensions.width` corresponds to the distance between `field-image.top` and `field-image.bottom`.

Multiple field JSON files may refer to the same `field-image.path` when variants share the same image but have different metadata, such as different official field dimensions.

The generated built-in Java and C++ field APIs expose the image path as a resource path relative to `/org/wpilib/fields/`, such as `frc/2024-crescendo.png`, so consumers can correlate field metadata with the separate image resources without reparsing JSON.

If a field needs a non-default generated selector name, a fixed ordering entry, or to become the default field, update `src/generate/fields.json`. The generated Java and C++ source text is defined by the Jinja templates under `src/generate/main`.

### Pregeneration

After adding or updating field JSON, run:

```bash
bazel run //fields:write_fields
```

Alternatively, the generator can be run directly from the repository root:

```bash
python fields/generate_fields.py
```

The direct Python path requires Jinja2 in the active Python environment. This updates the pregenerated Java enum and C++ registries in `src/generated`. The generated source exposes the standard JSON fields directly while retaining access to the embedded raw JSON resources for any additional data.

## Adding AprilTag layouts

Built-in AprilTag layouts live in the `field-tags` list of the matching field JSON file. The `field-dimensions` values are measured in meters and should match the dimensions used by the official layout.

For layouts sourced from official field layout CSVs:

1. Add or update the CSV in `src/generate/resources/org/wpilib/fields/apriltag`.
2. Run `python fields/convert_apriltag_layouts.py`.
3. Update the field JSON dimensions:
    1. Length is in meters from alliance wall to alliance wall.
    2. Width is in meters from inside guardrail plastic to plastic.
4. Run the fields pregenerator.

For fields with multiple official layouts, such as 2025 and 2026, add one field JSON per variant. The variant JSON files can share the same `field-image` while exposing separate `field-tags` and dimensions.
