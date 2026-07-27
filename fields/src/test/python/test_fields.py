import robotpy_fields


def test_builtin_field():
    field = robotpy_fields.get_field(robotpy_fields.FieldId.FRC_2024_CRESCENDO)

    assert field.get_name() == "2024 FRC Crescendo"
    assert field.get_game() == "Crescendo"
    assert field.get_resource_file() == "frc/2024-crescendo.json"
    assert field.has_tags()
    assert field.get_tag_pose(1) is not None


def test_field_image_metadata():
    field = robotpy_fields.get_field(robotpy_fields.FieldId.FRC_2024_CRESCENDO)
    image = field.get_image()

    assert image is not None
    assert image.get_path() == "frc/2024-crescendo.png"
    assert image.get_left() < image.get_right()
    assert image.get_top() < image.get_bottom()
