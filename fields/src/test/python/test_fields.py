import json
import math

import pytest
import robotpy_fields
from wpimath import Pose3d, Rotation3d

INCH_TO_METER = 0.0254
FOOT_TO_METER = 0.3048

ALL_FIELDS = [
    robotpy_fields.FieldId.FRC_2018_POWER_UP,
    robotpy_fields.FieldId.FRC_2019_DEEP_SPACE,
    robotpy_fields.FieldId.FRC_2020_INFINITE_RECHARGE,
    robotpy_fields.FieldId.FRC_2021_INFINITE_RECHARGE,
    robotpy_fields.FieldId.FRC_2021_BARREL,
    robotpy_fields.FieldId.FRC_2021_BOUNCE,
    robotpy_fields.FieldId.FRC_2021_GALACTIC_SEARCH_A,
    robotpy_fields.FieldId.FRC_2021_GALACTIC_SEARCH_B,
    robotpy_fields.FieldId.FRC_2021_SLALOM,
    robotpy_fields.FieldId.FRC_2022_RAPID_REACT,
    robotpy_fields.FieldId.FRC_2023_CHARGED_UP,
    robotpy_fields.FieldId.FRC_2024_CRESCENDO,
    robotpy_fields.FieldId.FTC_2024_2025_INTO_THE_DEEP,
    robotpy_fields.FieldId.FRC_2025_REEFSCAPE_WELDED,
    robotpy_fields.FieldId.FRC_2025_REEFSCAPE_ANDY_MARK,
    robotpy_fields.FieldId.FTC_2025_2026_DECODE,
    robotpy_fields.FieldId.FRC_2026_REBUILT_WELDED,
    robotpy_fields.FieldId.FRC_2026_REBUILT_ANDY_MARK,
]


def inches_to_meters(value):
    return value * INCH_TO_METER


def feet_to_meters(value):
    return value * FOOT_TO_METER


def assert_pose_close(actual, expected):
    assert actual is not None
    assert actual.x == pytest.approx(expected.x)
    assert actual.y == pytest.approx(expected.y)
    assert actual.z == pytest.approx(expected.z)

    actual_rotation = actual.rotation()
    expected_rotation = expected.rotation()
    assert angle_modulus(actual_rotation.x - expected_rotation.x) == pytest.approx(0.0)
    assert angle_modulus(actual_rotation.y - expected_rotation.y) == pytest.approx(0.0)
    assert angle_modulus(actual_rotation.z - expected_rotation.z) == pytest.approx(0.0)


def angle_modulus(angle):
    return math.atan2(math.sin(angle), math.cos(angle))


def make_test_field():
    return robotpy_fields.Field(
        "2027 FRC Test Field",
        "2027",
        "Test Field",
        None,
        feet_to_meters(54),
        feet_to_meters(27),
        "frc",
        [
            robotpy_fields.FieldTag(1, Pose3d(0, 0, 0, Rotation3d())),
            robotpy_fields.FieldTag(
                2,
                Pose3d(
                    feet_to_meters(4),
                    feet_to_meters(4),
                    feet_to_meters(4),
                    Rotation3d(0, 0, math.radians(180)),
                ),
            ),
        ],
    )


def test_get_fields_contains_entire_enum():
    assert set(robotpy_fields.get_fields()) == set(ALL_FIELDS)


def test_builtin_field():
    field = robotpy_fields.get_field(robotpy_fields.FieldId.FRC_2024_CRESCENDO)

    assert field.get_name() == "2024 FRC Crescendo"
    assert field.get_season() == "2024"
    assert field.get_game() == "Crescendo"
    assert field.get_program() == "frc"
    assert field.get_resource_file() == "frc/2024-crescendo.json"
    assert field.has_tags()
    assert field.get_tag_pose(1) is not None


@pytest.mark.parametrize("field_id", ALL_FIELDS)
def test_builtin_field_metadata(field_id):
    field = robotpy_fields.get_field(field_id)

    assert field.get_name() == robotpy_fields.get_field_name(field_id)
    assert field.get_season()
    assert field.get_game()
    assert field.get_program()
    assert field.get_resource_file().endswith(".json")
    assert field.get_length() > 0.0
    assert field.get_width() > 0.0
    assert field.has_tags() == bool(field.get_tags())

    raw_json = field.get_json()
    assert raw_json

    parsed_json = json.loads(raw_json)
    assert parsed_json["name"] == field.get_name()
    assert parsed_json["season"] == field.get_season()
    assert parsed_json["game"] == field.get_game()
    assert parsed_json["program"] == field.get_program()
    assert parsed_json["field-dimensions"]["length"] == pytest.approx(
        field.get_length()
    )
    assert parsed_json["field-dimensions"]["width"] == pytest.approx(field.get_width())

    image = field.get_image()
    assert field.has_image() == (image is not None)
    if image is None:
        assert "field-image" not in parsed_json
    else:
        parsed_image = parsed_json["field-image"]
        assert f"{field.get_program()}/{parsed_image['path']}" == image.get_path()
        assert parsed_image["top"] == image.get_top()
        assert parsed_image["left"] == image.get_left()
        assert parsed_image["bottom"] == image.get_bottom()
        assert parsed_image["right"] == image.get_right()
        assert image.get_path()
        assert image.get_left() < image.get_right()
        assert image.get_top() < image.get_bottom()


def test_field_image_metadata():
    field = robotpy_fields.get_field(robotpy_fields.FieldId.FRC_2024_CRESCENDO)
    image = field.get_image()
    assert image is not None
    assert image.get_path() == "frc/2024-crescendo.png"
    assert image.get_left() < image.get_right()
    assert image.get_top() < image.get_bottom()


def test_field_image_repr():
    image = robotpy_fields.FieldImage("frc/2024-crescendo.png", 10, 20, 30, 40)

    assert repr(image) == (
        "FieldImage(path='frc/2024-crescendo.png', "
        "top=10, left=20, bottom=30, right=40)"
    )


def test_field_tag_repr():
    pose = Pose3d(1, 2, 3, Rotation3d())
    tag = robotpy_fields.FieldTag(1, pose)

    assert repr(tag) == f"FieldTag(ID=1, pose={pose!r})"


def test_2022_rapid_react_tag_poses():
    field = robotpy_fields.get_field(robotpy_fields.FieldId.FRC_2022_RAPID_REACT)

    # Blue Hangar Truss - Hub
    expected_pose = Pose3d(
        inches_to_meters(127.272),
        inches_to_meters(216.01),
        inches_to_meters(67.932),
        Rotation3d(),
    )
    assert_pose_close(field.get_tag_pose(1), expected_pose)

    # Blue Terminal Near Station
    expected_pose = Pose3d(
        inches_to_meters(4.768),
        inches_to_meters(67.631),
        inches_to_meters(35.063),
        Rotation3d(0, 0, math.radians(46.25)),
    )
    assert_pose_close(field.get_tag_pose(5), expected_pose)

    # Upper Hub Blue-Near
    expected_pose = Pose3d(
        inches_to_meters(332.321),
        inches_to_meters(183.676),
        inches_to_meters(95.186),
        Rotation3d(0, math.radians(26.75), math.radians(69)),
    )
    assert_pose_close(field.get_tag_pose(53), expected_pose)

    assert field.get_tag_pose(54) is None


def test_set_origin_transformation_matches():
    field = make_test_field()

    field.set_origin(robotpy_fields.Field.OriginPosition.RED_ALLIANCE_WALL_RIGHT_SIDE)

    expected_pose = Pose3d(
        feet_to_meters(54),
        feet_to_meters(27),
        0,
        Rotation3d(0, 0, math.radians(180)),
    )
    assert_pose_close(field.get_tag_pose(1), expected_pose)

    expected_pose = Pose3d(
        feet_to_meters(50),
        feet_to_meters(23),
        feet_to_meters(4),
        Rotation3d(),
    )
    assert_pose_close(field.get_tag_pose(2), expected_pose)


def test_deserialize_round_trip(tmp_path):
    field = make_test_field()
    path = tmp_path / "field.json"

    field.serialize(str(path))
    deserialized = robotpy_fields.Field(str(path))

    assert field == deserialized


def test_load_field_without_dimensions_fails(tmp_path):
    path = tmp_path / "dimensionless.json"
    path.write_text(
        """{
  "name": "2027 FRC Dimensionless Field",
  "season": "2027",
  "game": "Dimensionless Field",
  "program": "frc"
}
""",
        encoding="utf-8",
    )

    with pytest.raises(Exception):
        robotpy_fields.Field(str(path))


def test_load_field_image_without_path_fails(tmp_path):
    path = tmp_path / "pathless-image.json"
    path.write_text(
        """{
  "name": "2027 FRC Pathless Image Field",
  "season": "2027",
  "game": "Pathless Image Field",
  "field-dimensions": {
    "length": 3.0,
    "width": 2.0
  },
  "field-image": {
    "top": 10,
    "left": 20,
    "bottom": 30,
    "right": 40
  },
  "program": "frc"
}
""",
        encoding="utf-8",
    )

    with pytest.raises(Exception):
        robotpy_fields.Field(str(path))


def test_load_field_without_image(tmp_path):
    field_json = """{
  "name": "2027 FRC Imageless Field",
  "season": "2027",
  "game": "Imageless Field",
  "field-dimensions": {
    "length": 3.0,
    "width": 2.0
  },
  "program": "frc"
}
"""
    path = tmp_path / "imageless.json"
    path.write_text(field_json, encoding="utf-8")

    field = robotpy_fields.Field(str(path))

    assert field.get_name() == "2027 FRC Imageless Field"
    assert field.get_season() == "2027"
    assert field.get_game() == "Imageless Field"
    assert not field.has_image()
    assert field.get_image() is None
    assert not field.has_tags()
    assert field.get_tags() == []
    assert field.get_length() == pytest.approx(3.0)
    assert field.get_width() == pytest.approx(2.0)
    assert field.get_json() == field_json
