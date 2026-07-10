import math

import pytest
from wpimath import Rotation2d

from wpilib import MecanumDrive


class MockMotorController:
    def __init__(self):
        self.throttle = 0

    def set_throttle(self, throttle):
        self.throttle = throttle

    def get_throttle(self):
        return self.throttle


def test_cartesian_ik():
    # Forward
    v = MecanumDrive.drive_cartesian_ik(1.0, 0.0, 0.0)
    assert v.front_left == pytest.approx(1.0)
    assert v.front_right == pytest.approx(1.0)
    assert v.rear_left == pytest.approx(1.0)
    assert v.rear_right == pytest.approx(1.0)

    # Left
    v = MecanumDrive.drive_cartesian_ik(0.0, -1.0, 0.0)
    assert v.front_left == pytest.approx(-1.0)
    assert v.front_right == pytest.approx(1.0)
    assert v.rear_left == pytest.approx(1.0)
    assert v.rear_right == pytest.approx(-1.0)

    # Right
    v = MecanumDrive.drive_cartesian_ik(0.0, 1.0, 0.0)
    assert v.front_left == pytest.approx(1.0)
    assert v.front_right == pytest.approx(-1.0)
    assert v.rear_left == pytest.approx(-1.0)
    assert v.rear_right == pytest.approx(1.0)

    # Rotate CCW
    v = MecanumDrive.drive_cartesian_ik(0.0, 0.0, -1.0)
    assert v.front_left == pytest.approx(-1.0)
    assert v.front_right == pytest.approx(1.0)
    assert v.rear_left == pytest.approx(-1.0)
    assert v.rear_right == pytest.approx(1.0)

    # Rotate CW
    v = MecanumDrive.drive_cartesian_ik(0.0, 0.0, 1.0)
    assert v.front_left == pytest.approx(1.0)
    assert v.front_right == pytest.approx(-1.0)
    assert v.rear_left == pytest.approx(1.0)
    assert v.rear_right == pytest.approx(-1.0)


def test_cartesian_ik_gyro_90_cw():
    gyro = Rotation2d.from_degrees(90)

    # Forward in global frame; left in robot frame
    v = MecanumDrive.drive_cartesian_ik(1.0, 0.0, 0.0, gyro)
    assert v.front_left == pytest.approx(-1.0)
    assert v.front_right == pytest.approx(1.0)
    assert v.rear_left == pytest.approx(1.0)
    assert v.rear_right == pytest.approx(-1.0)

    # Left in global frame; backward in robot frame
    v = MecanumDrive.drive_cartesian_ik(0.0, -1.0, 0.0, gyro)
    assert v.front_left == pytest.approx(-1.0)
    assert v.front_right == pytest.approx(-1.0)
    assert v.rear_left == pytest.approx(-1.0)
    assert v.rear_right == pytest.approx(-1.0)

    # Right in global frame; forward in robot frame
    v = MecanumDrive.drive_cartesian_ik(0.0, 1.0, 0.0, gyro)
    assert v.front_left == pytest.approx(1.0)
    assert v.front_right == pytest.approx(1.0)
    assert v.rear_left == pytest.approx(1.0)
    assert v.rear_right == pytest.approx(1.0)

    # Rotate CCW
    v = MecanumDrive.drive_cartesian_ik(0.0, 0.0, -1.0, gyro)
    assert v.front_left == pytest.approx(-1.0)
    assert v.front_right == pytest.approx(1.0)
    assert v.rear_left == pytest.approx(-1.0)
    assert v.rear_right == pytest.approx(1.0)

    # Rotate CW
    v = MecanumDrive.drive_cartesian_ik(0.0, 0.0, 1.0, gyro)
    assert v.front_left == pytest.approx(1.0)
    assert v.front_right == pytest.approx(-1.0)
    assert v.rear_left == pytest.approx(1.0)
    assert v.rear_right == pytest.approx(-1.0)


def test_cartesian(wpilib_state):
    fl = MockMotorController()
    rl = MockMotorController()
    fr = MockMotorController()
    rr = MockMotorController()
    drive = MecanumDrive(
        fl.set_throttle,
        rl.set_throttle,
        fr.set_throttle,
        rr.set_throttle,
    )
    drive.set_deadband(0.0)

    # Forward
    drive.drive_cartesian(1.0, 0.0, 0.0)
    assert fl.get_throttle() == pytest.approx(1.0)
    assert fr.get_throttle() == pytest.approx(1.0)
    assert rl.get_throttle() == pytest.approx(1.0)
    assert rr.get_throttle() == pytest.approx(1.0)

    # Left
    drive.drive_cartesian(0.0, -1.0, 0.0)
    assert fl.get_throttle() == pytest.approx(-1.0)
    assert fr.get_throttle() == pytest.approx(1.0)
    assert rl.get_throttle() == pytest.approx(1.0)
    assert rr.get_throttle() == pytest.approx(-1.0)

    # Right
    drive.drive_cartesian(0.0, 1.0, 0.0)
    assert fl.get_throttle() == pytest.approx(1.0)
    assert fr.get_throttle() == pytest.approx(-1.0)
    assert rl.get_throttle() == pytest.approx(-1.0)
    assert rr.get_throttle() == pytest.approx(1.0)

    # Rotate CCW
    drive.drive_cartesian(0.0, 0.0, -1.0)
    assert fl.get_throttle() == pytest.approx(-1.0)
    assert fr.get_throttle() == pytest.approx(1.0)
    assert rl.get_throttle() == pytest.approx(-1.0)
    assert rr.get_throttle() == pytest.approx(1.0)

    # Rotate CW
    drive.drive_cartesian(0.0, 0.0, 1.0)
    assert fl.get_throttle() == pytest.approx(1.0)
    assert fr.get_throttle() == pytest.approx(-1.0)
    assert rl.get_throttle() == pytest.approx(1.0)
    assert rr.get_throttle() == pytest.approx(-1.0)


def test_cartesian_gyro_90_cw(wpilib_state):
    fl = MockMotorController()
    rl = MockMotorController()
    fr = MockMotorController()
    rr = MockMotorController()
    drive = MecanumDrive(
        fl.set_throttle,
        rl.set_throttle,
        fr.set_throttle,
        rr.set_throttle,
    )
    drive.set_deadband(0.0)

    gyro = Rotation2d.from_degrees(90)

    # Forward in global frame; left in robot frame
    drive.drive_cartesian(1.0, 0.0, 0.0, gyro)
    assert fl.get_throttle() == pytest.approx(-1.0)
    assert fr.get_throttle() == pytest.approx(1.0)
    assert rl.get_throttle() == pytest.approx(1.0)
    assert rr.get_throttle() == pytest.approx(-1.0)

    # Left in global frame; backward in robot frame
    drive.drive_cartesian(0.0, -1.0, 0.0, gyro)
    assert fl.get_throttle() == pytest.approx(-1.0)
    assert fr.get_throttle() == pytest.approx(-1.0)
    assert rl.get_throttle() == pytest.approx(-1.0)
    assert rr.get_throttle() == pytest.approx(-1.0)

    # Right in global frame; forward in robot frame
    drive.drive_cartesian(0.0, 1.0, 0.0, gyro)
    assert fl.get_throttle() == pytest.approx(1.0)
    assert fr.get_throttle() == pytest.approx(1.0)
    assert rl.get_throttle() == pytest.approx(1.0)
    assert rr.get_throttle() == pytest.approx(1.0)

    # Rotate CCW
    drive.drive_cartesian(0.0, 0.0, -1.0, gyro)
    assert fl.get_throttle() == pytest.approx(-1.0)
    assert fr.get_throttle() == pytest.approx(1.0)
    assert rl.get_throttle() == pytest.approx(-1.0)
    assert rr.get_throttle() == pytest.approx(1.0)

    # Rotate CW
    drive.drive_cartesian(0.0, 0.0, 1.0, gyro)
    assert fl.get_throttle() == pytest.approx(1.0)
    assert fr.get_throttle() == pytest.approx(-1.0)
    assert rl.get_throttle() == pytest.approx(1.0)
    assert rr.get_throttle() == pytest.approx(-1.0)


def test_polar(wpilib_state):
    fl = MockMotorController()
    rl = MockMotorController()
    fr = MockMotorController()
    rr = MockMotorController()
    drive = MecanumDrive(
        fl.set_throttle,
        rl.set_throttle,
        fr.set_throttle,
        rr.set_throttle,
    )
    drive.set_deadband(0.0)

    # Forward
    drive.drive_polar(1.0, Rotation2d.from_degrees(0), 0.0)
    assert fl.get_throttle() == pytest.approx(1.0)
    assert fr.get_throttle() == pytest.approx(1.0)
    assert rl.get_throttle() == pytest.approx(1.0)
    assert rr.get_throttle() == pytest.approx(1.0)

    # Left
    drive.drive_polar(1.0, Rotation2d.from_degrees(-90), 0.0)
    assert fl.get_throttle() == pytest.approx(-1.0)
    assert fr.get_throttle() == pytest.approx(1.0)
    assert rl.get_throttle() == pytest.approx(1.0)
    assert rr.get_throttle() == pytest.approx(-1.0)

    # Right
    drive.drive_polar(1.0, Rotation2d.from_degrees(90), 0.0)
    assert fl.get_throttle() == pytest.approx(1.0)
    assert fr.get_throttle() == pytest.approx(-1.0)
    assert rl.get_throttle() == pytest.approx(-1.0)
    assert rr.get_throttle() == pytest.approx(1.0)

    # Rotate CCW
    drive.drive_polar(0.0, Rotation2d.from_degrees(0), -1.0)
    assert fl.get_throttle() == pytest.approx(-1.0)
    assert fr.get_throttle() == pytest.approx(1.0)
    assert rl.get_throttle() == pytest.approx(-1.0)
    assert rr.get_throttle() == pytest.approx(1.0)

    # Rotate CW
    drive.drive_polar(0.0, Rotation2d.from_degrees(0), 1.0)
    assert fl.get_throttle() == pytest.approx(1.0)
    assert fr.get_throttle() == pytest.approx(-1.0)
    assert rl.get_throttle() == pytest.approx(1.0)
    assert rr.get_throttle() == pytest.approx(-1.0)
