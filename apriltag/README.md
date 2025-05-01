# AprilTag

## Adding new field to AprilTagFields

### Adding field JSON

1. Add a field layout CSV file to `src/main/native/resources/edu/wpi/first/apriltag`
    1. See docstring in `convert_apriltag_layouts.py` for more
2. Run `convert_apriltag_layouts.py` in the same directory as this readme to generate the JSON
3. That script overwrites all generated JSONs, so undo undesired changes if necessary
4. Update the field dimensions at the bottom of the JSON
    1. Length should be in meters from alliance wall to alliance wall
    2. Width should be in meters from inside guardrail plastic to plastic

### Java updates

1. Update `src/main/java/edu/wpi/first/apriltag/AprilTagFields.java`
    1. Add enum value for new field to `AprilTagFields`
    2. Update `AprilTagFields.DEFAULT_FIELD` if necessary

### C++ updates

1. Update `src/main/native/include/frc/apriltag/AprilTagFields.h`
    1. Add enum value for new field to `AprilTagFields`
    2. Update `AprilTagFields::DEFAULT_FIELD` if necessary
2. Update `src/main/native/cpp/AprilTagFields.cpp`
    1. Add resource getter prototype like `std::string_view GetResource_2024_crescendo_json()`
    2. Add case for new field to switch in `LoadAprilTagLayoutField()`
