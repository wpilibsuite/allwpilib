import pytest
import math
import numpy as np

from wpimath.geometry import (
    CoordinateSystem,
    Pose3d,
    Transform3d,
    Translation3d,
    Rotation3d,
)


def check_pose3d_convert(pose_from, pose_to, coord_from, coord_to):
    # "from" to "to"
    assert pose_to.translation() == CoordinateSystem.convert(
        pose_from.translation(), coord_from, coord_to
    )
    assert pose_to.rotation() == CoordinateSystem.convert(
        pose_from.rotation(), coord_from, coord_to
    )
    assert pose_to == CoordinateSystem.convert(pose_from, coord_from, coord_to)

    # "to" to "from"
    assert pose_from.translation() == CoordinateSystem.convert(
        pose_to.translation(), coord_to, coord_from
    )
    assert pose_from.rotation() == CoordinateSystem.convert(
        pose_to.rotation(), coord_to, coord_from
    )
    assert pose_from == CoordinateSystem.convert(pose_to, coord_to, coord_from)


def check_transform3d_convert(transform_from, transform_to, coord_from, coord_to):
    # "from" to "to"
    assert transform_to.translation() == CoordinateSystem.convert(
        transform_from.translation(), coord_from, coord_to
    )
    assert transform_to == CoordinateSystem.convert(
        transform_from, coord_from, coord_to
    )

    # "to" to "from"
    assert transform_from.translation() == CoordinateSystem.convert(
        transform_to.translation(), coord_to, coord_from
    )
    assert transform_from == CoordinateSystem.convert(
        transform_to, coord_to, coord_from
    )


def test_pose3d_edn_to_nwu():
    # No rotation from EDN to NWU
    check_pose3d_convert(
        Pose3d(x=1, y=2, z=3, rotation=Rotation3d()),
        Pose3d(x=3, y=-1, z=-2, rotation=Rotation3d.fromDegrees(-90, 0, -90)),
        CoordinateSystem.EDN(),
        CoordinateSystem.NWU(),
    )

    # 45° roll from EDN to NWU
    check_pose3d_convert(
        Pose3d(x=1, y=2, z=3, rotation=Rotation3d.fromDegrees(45, 0, 0)),
        Pose3d(x=3, y=-1, z=-2, rotation=Rotation3d.fromDegrees(-45, 0, -90)),
        CoordinateSystem.EDN(),
        CoordinateSystem.NWU(),
    )

    # 45° pitch from EDN to NWU
    check_pose3d_convert(
        Pose3d(x=1, y=2, z=3, rotation=Rotation3d.fromDegrees(0, 45, 0)),
        Pose3d(x=3, y=-1, z=-2, rotation=Rotation3d.fromDegrees(-90, 0, -135)),
        CoordinateSystem.EDN(),
        CoordinateSystem.NWU(),
    )

    # 45° yaw from EDN to NWU
    check_pose3d_convert(
        Pose3d(x=1, y=2, z=3, rotation=Rotation3d.fromDegrees(0, 0, 45)),
        Pose3d(x=3, y=-1, z=-2, rotation=Rotation3d.fromDegrees(-90, 45, -90)),
        CoordinateSystem.EDN(),
        CoordinateSystem.NWU(),
    )


def test_pose3d_edn_to_ned():
    # No rotation from EDN to NED
    check_pose3d_convert(
        Pose3d(x=1, y=2, z=3, rotation=Rotation3d()),
        Pose3d(x=3, y=1, z=2, rotation=Rotation3d.fromDegrees(90, 0, 90)),
        CoordinateSystem.EDN(),
        CoordinateSystem.NED(),
    )

    # 45° roll from EDN to NED
    check_pose3d_convert(
        Pose3d(x=1, y=2, z=3, rotation=Rotation3d.fromDegrees(45, 0, 0)),
        Pose3d(x=3, y=1, z=2, rotation=Rotation3d.fromDegrees(135, 0, 90)),
        CoordinateSystem.EDN(),
        CoordinateSystem.NED(),
    )

    # 45° pitch from EDN to NED
    check_pose3d_convert(
        Pose3d(x=1, y=2, z=3, rotation=Rotation3d.fromDegrees(0, 45, 0)),
        Pose3d(x=3, y=1, z=2, rotation=Rotation3d.fromDegrees(90, 0, 135)),
        CoordinateSystem.EDN(),
        CoordinateSystem.NED(),
    )

    # 45° yaw from EDN to NED
    check_pose3d_convert(
        Pose3d(x=1, y=2, z=3, rotation=Rotation3d.fromDegrees(0, 0, 45)),
        Pose3d(x=3, y=1, z=2, rotation=Rotation3d.fromDegrees(90, -45, 90)),
        CoordinateSystem.EDN(),
        CoordinateSystem.NED(),
    )


def test_transform3d_edn_to_nwu():
    # No rotation from EDN to NWU
    check_transform3d_convert(
        Transform3d(Translation3d(x=1, y=2, z=3), rotation=Rotation3d()),
        Transform3d(Translation3d(x=3, y=-1, z=-2), rotation=Rotation3d()),
        CoordinateSystem.EDN(),
        CoordinateSystem.NWU(),
    )

    # 45° roll from EDN to NWU
    check_transform3d_convert(
        Transform3d(
            Translation3d(x=1, y=2, z=3),
            Rotation3d.fromDegrees(45, 0, 0),
        ),
        Transform3d(
            Translation3d(x=3, y=-1, z=-2),
            Rotation3d.fromDegrees(0, -45, 0),
        ),
        CoordinateSystem.EDN(),
        CoordinateSystem.NWU(),
    )

    # 45° pitch from EDN to NWU
    check_transform3d_convert(
        Transform3d(
            Translation3d(x=1, y=2, z=3),
            Rotation3d.fromDegrees(0, 45, 0),
        ),
        Transform3d(
            Translation3d(x=3, y=-1, z=-2),
            Rotation3d.fromDegrees(0, 0, -45),
        ),
        CoordinateSystem.EDN(),
        CoordinateSystem.NWU(),
    )

    # 45° yaw from EDN to NWU
    check_transform3d_convert(
        Transform3d(
            Translation3d(x=1, y=2, z=3),
            Rotation3d.fromDegrees(0, 0, 45),
        ),
        Transform3d(
            Translation3d(x=3, y=-1, z=-2),
            Rotation3d.fromDegrees(45, 0, 0),
        ),
        CoordinateSystem.EDN(),
        CoordinateSystem.NWU(),
    )


def test_transform3d_edn_to_ned():
    # No rotation from EDN to NED
    check_transform3d_convert(
        Transform3d(Translation3d(x=1, y=2, z=3), rotation=Rotation3d()),
        Transform3d(Translation3d(x=3, y=1, z=2), rotation=Rotation3d()),
        CoordinateSystem.EDN(),
        CoordinateSystem.NED(),
    )

    # 45° roll from EDN to NED
    check_transform3d_convert(
        Transform3d(
            Translation3d(x=1, y=2, z=3),
            Rotation3d.fromDegrees(45, 0, 0),
        ),
        Transform3d(
            Translation3d(x=3, y=1, z=2),
            Rotation3d.fromDegrees(0, 45, 0),
        ),
        CoordinateSystem.EDN(),
        CoordinateSystem.NED(),
    )

    # 45° pitch from EDN to NED
    check_transform3d_convert(
        Transform3d(
            Translation3d(x=1, y=2, z=3),
            Rotation3d.fromDegrees(0, 45, 0),
        ),
        Transform3d(
            Translation3d(x=3, y=1, z=2),
            Rotation3d.fromDegrees(0, 0, 45),
        ),
        CoordinateSystem.EDN(),
        CoordinateSystem.NED(),
    )

    # 45° yaw from EDN to NED
    check_transform3d_convert(
        Transform3d(
            Translation3d(x=1, y=2, z=3),
            Rotation3d.fromDegrees(0, 0, 45),
        ),
        Transform3d(
            Translation3d(x=3, y=1, z=2),
            Rotation3d.fromDegrees(45, 0, 0),
        ),
        CoordinateSystem.EDN(),
        CoordinateSystem.NED(),
    )
