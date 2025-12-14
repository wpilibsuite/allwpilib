import pytest
import math
import numpy as np

from wpimath import ChassisVelocities, Twist2d, Pose2d, Rotation2d


def test_discretize():
    target = ChassisVelocities(vx=1, vy=0, omega=0.5)
    duration = 1
    dt = 0.01

    velocities = target.discretize(duration)
    twist = Twist2d(velocities.vx * dt, velocities.vy * dt, velocities.omega * dt)

    pose = Pose2d()
    time = 0
    while time < duration:
        pose = pose + twist.exp()
        time += dt

    assert pose.x == pytest.approx((target.vx * duration), abs=1e-9)
    assert pose.y == pytest.approx((target.vy * duration), abs=1e-9)
    assert pose.rotation().radians() == pytest.approx(
        (target.omega * duration), abs=1e-9
    )


def test_to_robot_relative():
    chassis_velocities = ChassisVelocities(vx=1, vy=0, omega=0.5).toRobotRelative(
        Rotation2d.fromDegrees(-90)
    )

    assert chassis_velocities.vx == pytest.approx(0.0, abs=1e-9)
    assert chassis_velocities.vy == pytest.approx(1.0, abs=1e-9)
    assert chassis_velocities.omega == pytest.approx(0.5, abs=1e-9)


def test_to_field_relative():
    chassis_velocities = ChassisVelocities(vx=1, vy=0, omega=0.5).toFieldRelative(
        Rotation2d.fromDegrees(45)
    )

    assert chassis_velocities.vx == pytest.approx(1.0 / math.sqrt(2.0), abs=1e-9)
    assert chassis_velocities.vy == pytest.approx(1.0 / math.sqrt(2.0), abs=1e-9)
    assert chassis_velocities.omega == pytest.approx(0.5, abs=1e-9)


def test_plus() -> None:
    left = ChassisVelocities(vx=1.0, vy=0.5, omega=0.75)
    right = ChassisVelocities(vx=2.0, vy=1.5, omega=0.25)

    result = left + right

    assert math.isclose(3.0, result.vx)
    assert math.isclose(2.0, result.vy)
    assert math.isclose(1.0, result.omega)


def test_minus() -> None:
    left = ChassisVelocities(vx=1.0, vy=0.5, omega=0.75)
    right = ChassisVelocities(vx=2.0, vy=0.5, omega=0.25)

    result = left - right

    assert math.isclose(-1.0, result.vx)
    assert math.isclose(0, result.vy)
    assert math.isclose(0.5, result.omega)


def test_neg() -> None:
    velocities = ChassisVelocities(vx=1.0, vy=0.5, omega=0.75)

    result = -velocities

    assert math.isclose(-1.0, result.vx)
    assert math.isclose(-0.5, result.vy)
    assert math.isclose(-0.75, result.omega)


def test_multiplication() -> None:
    velocities = ChassisVelocities(vx=1.0, vy=0.5, omega=0.75)

    result = velocities * 2

    assert math.isclose(2.0, result.vx)
    assert math.isclose(1.0, result.vy)
    assert math.isclose(1.5, result.omega)


def test_division() -> None:
    velocities = ChassisVelocities(vx=1.0, vy=0.5, omega=0.75)

    result = velocities / 2

    assert math.isclose(0.5, result.vx)
    assert math.isclose(0.25, result.vy)
    assert math.isclose(0.375, result.omega)


def test_unpack() -> None:
    velocities = ChassisVelocities(1, 1, 0.5)

    vx, vy, omega = velocities

    assert math.isclose(1, vx)
    assert math.isclose(1, vy)
    assert math.isclose(0.5, omega)


def test_repr() -> None:
    velocities = ChassisVelocities(2.0, 1.0, 0.0)

    assert repr(velocities) == "ChassisVelocities(vx=2.000000, vy=1.000000, omega=0.000000)"
