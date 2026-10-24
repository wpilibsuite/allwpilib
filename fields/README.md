# fields

The library where FIRST field metadata and optional field images are stored for use by other programs.

## Field coordinates

All field-relative `Pose2d` and `Pose3d` values use the center of the field on the
floor as the origin. +X points toward the blue alliance wall, +Y points left when
facing +X, and +Z points up. Zero heading faces +X; positive heading/yaw is
counterclockwise viewed from above. The convention is fixed regardless of the
robot's alliance. It applies to every built-in field, including prior seasons.

For a field of length L and width W, X spans [-L/2, L/2] and Y spans [-W/2, W/2].
AprilTag poses locate tag centers, and an upright tag with zero rotation faces
+X. Tags outside the playable boundary can have coordinates outside these ranges.

`Field` loads JSON and built-in tag data directly in this frame. `FIELD_CENTER`
is the default origin. Explicit wall origins face into the field: the blue
right-side corner is at (L/2, W/2, 0) with yaw 180 degrees, and the red right-side
corner is at (-L/2, -W/2, 0) with yaw zero. Custom origins are poses in the centered
frame. Selecting an origin affects tag pose queries, not the stored/serialized
layout.

This replaces the former blue-wall corner frame without automatic conversion.
To migrate existing field poses, use:

- x = L/2 - old_x
- y = W/2 - old_y
- z = old_z
- heading = old_heading + 180 degrees (modulo 360 degrees)
- quaternion (w, x, y, z) = (-old_z, -old_y, old_x, old_w)

The quaternion rule premultiplies by a half-turn about field Z, including for
pitched or rolled objects. Field-relative linear velocities and accelerations
also reverse X and Y; yaw rate is unchanged. Robot-relative geometry,
sensor mounting transforms, and the arithmetic of poses and transforms are
unchanged. Set odometry's initial pose and supply vision poses in the same frame.
A default pose now represents field center facing blue when interpreted as a
field pose; it does not locate a robot automatically.

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

Image pixel coordinates have X = 0 at the left edge increasing rightward, and Y = 0 at the top edge increasing downward. These are distinct from field coordinates. Glass displays +X field coordinates to the right and +Y upward, with (0, 0) at the midpoint of the field boundaries, excluding image margins. The built-in images are rotated 180 degrees from their previous orientation, and the field boundary metadata is measured in these rotated images. The field image must be oriented so that the field length runs along the horizontal left/right axis and the field width runs along the vertical top/bottom axis. In other words, `field-dimensions.length` corresponds to the distance between `field-image.left` and `field-image.right`, while `field-dimensions.width` corresponds to the distance between `field-image.top` and `field-image.bottom`.

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
   CSV poses use the centered field frame described above. Official CSVs from
   2027 onward use this convention; the checked-in 2025 and 2026 CSVs have been
   converted to it. Coordinates are in inches, `Z-Rotation` is yaw in degrees,
   and `Y-Rotation` is pitch in degrees.
2. Update the field JSON dimensions:
    1. Length is in meters from alliance wall to alliance wall.
    2. Width is in meters from inside guardrail plastic to plastic.
3. Run `python fields/convert_apriltag_layouts.py`. The converter changes inches
   to meters and Euler angles to quaternions without shifting or rotating the
   coordinate frame.
4. Run the fields pregenerator.

For fields with multiple official layouts, such as 2025 and 2026, add one field JSON per variant. The variant JSON files can share the same `field-image` while exposing separate `field-tags` and dimensions.
