import math

from wpimath.geometry import (
    Ellipse2d,
    Pose2d,
    Rectangle2d,
    Rotation2d,
    Transform2d,
    Translation2d,
)
from wpimath.spline import CubicHermiteSpline, SplineHelper
from wpimath.trajectory import (
    Trajectory,
    TrajectoryConfig,
    TrajectoryGenerator,
    TrajectoryParameterizer,
)
from wpimath.trajectory.constraint import (
    EllipticalRegionConstraint,
    MaxVelocityConstraint,
    RectangularRegionConstraint,
    TrajectoryConstraint,
)


def getTestTrajectory(config: TrajectoryConfig) -> Trajectory:
    # 2018 cross scale auto waypoints
    sideStart = Pose2d.fromFeet(1.54, 23.23, Rotation2d.fromDegrees(180))
    crossScale = Pose2d.fromFeet(23.7, 6.8, Rotation2d.fromDegrees(-160))

    config.setReversed(True)

    vector = [
        (
            sideStart + Transform2d(Translation2d.fromFeet(-13, 0), Rotation2d())
        ).translation(),
        (
            sideStart
            + Transform2d(
                Translation2d.fromFeet(-19.5, 5.0), Rotation2d.fromDegrees(-90)
            )
        ).translation(),
    ]

    return TrajectoryGenerator.generateTrajectory(sideStart, vector, crossScale, config)


#
# EllipticalRegionConstraint tests
#


def test_elliptical_region_constraint():
    maxVelocity = 2
    ellipse = Ellipse2d.fromFeet(Pose2d.fromFeet(5, 2.5, math.radians(180)), 5, 2.5)

    config = TrajectoryConfig.fromFps(13, 13)
    maxVelConstraint = MaxVelocityConstraint.fromFps(maxVelocity)
    regionConstraint = EllipticalRegionConstraint(ellipse, maxVelConstraint)

    config.addConstraint(regionConstraint)

    trajectory = getTestTrajectory(config)

    exceededConstraintOutsideRegion = False
    for point in trajectory.states():
        if ellipse.contains(point.pose.translation()):
            assert abs(point.velocity_fps) < maxVelocity + 0.05
        elif abs(point.velocity_fps) >= maxVelocity + 0.05:
            exceededConstraintOutsideRegion = True

        # translation = point.pose.translation()
        # if translation.x_feet < 10 and translation.y_feet < 5:
        #     assert abs(point.velocity_fps) < maxVelocity + 0.05
        # elif abs(point.velocity_fps) >= maxVelocity + 0.05:
        #     exceededConstraintOutsideRegion = True

    assert exceededConstraintOutsideRegion


#
# RectangularRegionConstraint tests
#


def test_rectangular_region_constraint():
    maxVelocity = 2
    rectangle = Rectangle2d(Translation2d.fromFeet(1, 1), Translation2d.fromFeet(5, 27))

    config = TrajectoryConfig.fromFps(13, 13)
    maxVelConstraint = MaxVelocityConstraint.fromFps(maxVelocity)
    regionConstraint = RectangularRegionConstraint(rectangle, maxVelConstraint)

    config.addConstraint(regionConstraint)

    trajectory = getTestTrajectory(config)

    exceededConstraintOutsideRegion = False
    for point in trajectory.states():
        if rectangle.contains(point.pose.translation()):
            assert abs(point.velocity_fps) < maxVelocity + 0.05
        elif abs(point.velocity_fps) >= maxVelocity + 0.05:
            exceededConstraintOutsideRegion = True

    assert exceededConstraintOutsideRegion


#
# TrajectoryConstraint
#


def test_trajectory_constraint_min_max():
    min_max = TrajectoryConstraint.MinMax()
    _, _ = min_max

    min_max = TrajectoryConstraint.MinMax(minAcceleration=0, maxAcceleration=1)
    assert (
        repr(min_max)
        == "TrajectoryConstraint.MinMax(minAcceleration=0.0, maxAcceleration=1.0)"
    )


#
# TrajectoryParameterizer
#


def test_trajectory_parameterizer():
    start = Pose2d(1, 1, 0)
    end = Pose2d(2, 2, math.pi / 2)

    # generate the spline from start and end poses
    vec1, vec2 = SplineHelper.cubicControlVectorsFromWaypoints(start, [], end)
    spline = CubicHermiteSpline(vec1.x, vec2.x, vec1.y, vec2.y)

    # sample the pose and curvature along the spline
    points: list[tuple[Pose2d, float]] = []
    for i in range(100):
        points.append(spline.getPoint(i / 100))

    trajectory = TrajectoryParameterizer.timeParameterizeTrajectory(
        points, [], 0, 0, 4, 3, False
    )
    assert trajectory is not None
