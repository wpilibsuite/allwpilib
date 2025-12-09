import pytest
import math
import numpy as np

from wpimath.kinematics import ChassisSpeeds
from wpimath.geometry import Twist2d, Pose2d, Rotation2d


def test_discretize():
    target = ChassisSpeeds(vx=1, vy=0, omega=0.5)
    duration = 1
    dt = 0.01

    speeds = target.discretize(duration)
    twist = Twist2d(speeds.vx * dt, speeds.vy * dt, speeds.omega * dt)

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
    chassis_speeds = ChassisSpeeds(vx=1, vy=0, omega=0.5).toRobotRelative(Rotation2d.fromDegrees(-90))
        

    assert chassis_speeds.vx == pytest.approx(0.0, abs=1e-9)
    assert chassis_speeds.vy == pytest.approx(1.0, abs=1e-9)
    assert chassis_speeds.omega == pytest.approx(0.5, abs=1e-9)


def test_to_field_relative():
    chassis_speeds = ChassisSpeeds(vx=1, vy=0, omega=0.5).toFieldRelative(Rotation2d.fromDegrees(45))
    
    assert chassis_speeds.vx == pytest.approx(1.0 / math.sqrt(2.0), abs=1e-9)
    assert chassis_speeds.vy == pytest.approx(1.0 / math.sqrt(2.0), abs=1e-9)
    assert chassis_speeds.omega == pytest.approx(0.5, abs=1e-9)


def test_plus() -> None:
    left = ChassisSpeeds(vx=1.0, vy=0.5, omega=0.75)
    right = ChassisSpeeds(vx=2.0, vy=1.5, omega=0.25)

    result = left + right

    assert math.isclose(3.0, result.vx)
    assert math.isclose(2.0, result.vy)
    assert math.isclose(1.0, result.omega)


def test_minus() -> None:
    left = ChassisSpeeds(vx=1.0, vy=0.5, omega=0.75)
    right = ChassisSpeeds(vx=2.0, vy=0.5, omega=0.25)

    result = left - right

    assert math.isclose(-1.0, result.vx)
    assert math.isclose(0, result.vy)
    assert math.isclose(0.5, result.omega)


def test_neg() -> None:
    speeds = ChassisSpeeds(vx=1.0, vy=0.5, omega=0.75)

    result = -speeds

    assert math.isclose(-1.0, result.vx)
    assert math.isclose(-0.5, result.vy)
    assert math.isclose(-0.75, result.omega)


def test_multiplication() -> None:
    speeds = ChassisSpeeds(vx=1.0, vy=0.5, omega=0.75)

    result = speeds * 2

    assert math.isclose(2.0, result.vx)
    assert math.isclose(1.0, result.vy)
    assert math.isclose(1.5, result.omega)


def test_division() -> None:
    speeds = ChassisSpeeds(vx=1.0, vy=0.5, omega=0.75)

    result = speeds / 2

    assert math.isclose(0.5, result.vx)
    assert math.isclose(0.25, result.vy)
    assert math.isclose(0.375, result.omega)


def test_unpack() -> None:
    speeds = ChassisSpeeds(1, 1, 0.5)

    vx, vy, omega = speeds

    assert math.isclose(1, vx)
    assert math.isclose(1, vy)
    assert math.isclose(0.5, omega)


def test_repr() -> None:
    speeds = ChassisSpeeds(2.0, 1.0, 0.0)

    assert repr(speeds) == "ChassisSpeeds(vx=2.000000, vy=1.000000, omega=0.000000)"
