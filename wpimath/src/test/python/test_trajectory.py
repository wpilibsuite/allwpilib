import math

from wpimath import (
    CubicHermiteSpline,
    DrivetrainSplineTrajectory,
    Ellipse2d,
    EllipticalRegionConstraint,
    MaxVelocityConstraint,
    Pose2d,
    Rectangle2d,
    RectangularRegionConstraint,
    Rotation2d,
    SplineHelper,
    TrajectoryConfig,
    TrajectoryConstraint,
    TrajectoryGenerator,
    TrajectoryParameterizer,
    Transform2d,
    Translation2d,
)


def get_test_trajectory(config: TrajectoryConfig) -> DrivetrainSplineTrajectory:
    # 2018 cross scale auto waypoints
    side_start = Pose2d.from_feet(1.54, 23.23, Rotation2d.from_degrees(180))
    cross_scale = Pose2d.from_feet(23.7, 6.8, Rotation2d.from_degrees(-160))

    config.set_reversed(True)

    vector = [
        (
            side_start + Transform2d(Translation2d.from_feet(-13, 0), Rotation2d())
        ).translation(),
        (
            side_start
            + Transform2d(
                Translation2d.from_feet(-19.5, 5.0), Rotation2d.from_degrees(-90)
            )
        ).translation(),
    ]

    return TrajectoryGenerator.generate_trajectory(
        side_start, vector, cross_scale, config
    )


#
# EllipticalRegionConstraint tests
#


def test_elliptical_region_constraint():
    max_velocity = 2
    ellipse = Ellipse2d.from_feet(Pose2d.from_feet(5, 2.5, math.radians(180)), 5, 2.5)

    config = TrajectoryConfig.from_fps(13, 13)
    max_vel_constraint = MaxVelocityConstraint.from_fps(max_velocity)
    region_constraint = EllipticalRegionConstraint(ellipse, max_vel_constraint)

    config.add_constraint(region_constraint)

    trajectory = get_test_trajectory(config)

    exceeded_constraint_outside_region = False
    for point in trajectory.samples():
        if ellipse.contains(point.pose.translation()):
            assert abs(point.forward_velocity()) < max_velocity + 0.05
        elif abs(point.forward_velocity()) >= max_velocity + 0.05:
            exceeded_constraint_outside_region = True

        # translation = point.pose.translation()
        # if translation.x_feet < 10 and translation.y_feet < 5:
        #     assert abs(point.velocity_fps) < maxVelocity + 0.05
        # elif abs(point.velocity_fps) >= maxVelocity + 0.05:
        #     exceededConstraintOutsideRegion = True

    assert exceeded_constraint_outside_region


#
# RectangularRegionConstraint tests
#


def test_rectangular_region_constraint():
    max_velocity = 2
    rectangle = Rectangle2d(
        Translation2d.from_feet(1, 1), Translation2d.from_feet(5, 27)
    )

    config = TrajectoryConfig.from_fps(13, 13)
    max_vel_constraint = MaxVelocityConstraint.from_fps(max_velocity)
    region_constraint = RectangularRegionConstraint(rectangle, max_vel_constraint)

    config.add_constraint(region_constraint)

    trajectory = get_test_trajectory(config)

    exceeded_constraint_outside_region = False
    for point in trajectory.samples():
        if rectangle.contains(point.pose.translation()):
            assert abs(point.forward_velocity()) < max_velocity + 0.05
        elif abs(point.forward_velocity()) >= max_velocity + 0.05:
            exceeded_constraint_outside_region = True

    assert exceeded_constraint_outside_region


#
# TrajectoryConstraint
#


def test_trajectory_constraint_min_max():
    min_max = TrajectoryConstraint.MinMax()
    _, _ = min_max

    min_max = TrajectoryConstraint.MinMax(min_acceleration=0, max_acceleration=1)
    assert (
        repr(min_max)
        == "TrajectoryConstraint.MinMax(min_acceleration=0.0, max_acceleration=1.0)"
    )


#
# TrajectoryParameterizer
#


def test_trajectory_parameterizer():
    start = Pose2d(1, 1, 0)
    end = Pose2d(2, 2, math.pi / 2)

    # generate the spline from start and end poses
    vec1, vec2 = SplineHelper.cubic_control_vectors_from_waypoints(start, [], end)
    spline = CubicHermiteSpline(vec1.x, vec2.x, vec1.y, vec2.y)

    # sample the pose and curvature along the spline
    points: list[tuple[Pose2d, float]] = []
    for i in range(100):
        points.append(spline.get_point(i / 100))

    trajectory = TrajectoryParameterizer.time_parameterize_trajectory(
        points, [], 0, 0, 4, 3, False
    )
    assert trajectory is not None
