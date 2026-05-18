import math

import pytest
from wpimath import Rotation2d

from wpilib import MecanumDrive


class MockMotorController:
    def __init__(self):
        self.throttle = 0

    def setThrottle(self, throttle):
        self.throttle = throttle

    def getThrottle(self):
        return self.throttle


def test_cartesian_ik():
    # Forward
    v = MecanumDrive.driveCartesianIK(1.0, 0.0, 0.0)
    assert v.frontLeft == pytest.approx(1.0)
    assert v.frontRight == pytest.approx(1.0)
    assert v.rearLeft == pytest.approx(1.0)
    assert v.rearRight == pytest.approx(1.0)

    # Left
    v = MecanumDrive.driveCartesianIK(0.0, -1.0, 0.0)
    assert v.frontLeft == pytest.approx(-1.0)
    assert v.frontRight == pytest.approx(1.0)
    assert v.rearLeft == pytest.approx(1.0)
    assert v.rearRight == pytest.approx(-1.0)

    # Right
    v = MecanumDrive.driveCartesianIK(0.0, 1.0, 0.0)
    assert v.frontLeft == pytest.approx(1.0)
    assert v.frontRight == pytest.approx(-1.0)
    assert v.rearLeft == pytest.approx(-1.0)
    assert v.rearRight == pytest.approx(1.0)

    # Rotate CCW
    v = MecanumDrive.driveCartesianIK(0.0, 0.0, -1.0)
    assert v.frontLeft == pytest.approx(-1.0)
    assert v.frontRight == pytest.approx(1.0)
    assert v.rearLeft == pytest.approx(-1.0)
    assert v.rearRight == pytest.approx(1.0)

    # Rotate CW
    v = MecanumDrive.driveCartesianIK(0.0, 0.0, 1.0)
    assert v.frontLeft == pytest.approx(1.0)
    assert v.frontRight == pytest.approx(-1.0)
    assert v.rearLeft == pytest.approx(1.0)
    assert v.rearRight == pytest.approx(-1.0)


def test_cartesian_ik_gyro_90_cw():
    gyro = Rotation2d.fromDegrees(90)

    # Forward in global frame; left in robot frame
    v = MecanumDrive.driveCartesianIK(1.0, 0.0, 0.0, gyro)
    assert v.frontLeft == pytest.approx(-1.0)
    assert v.frontRight == pytest.approx(1.0)
    assert v.rearLeft == pytest.approx(1.0)
    assert v.rearRight == pytest.approx(-1.0)

    # Left in global frame; backward in robot frame
    v = MecanumDrive.driveCartesianIK(0.0, -1.0, 0.0, gyro)
    assert v.frontLeft == pytest.approx(-1.0)
    assert v.frontRight == pytest.approx(-1.0)
    assert v.rearLeft == pytest.approx(-1.0)
    assert v.rearRight == pytest.approx(-1.0)

    # Right in global frame; forward in robot frame
    v = MecanumDrive.driveCartesianIK(0.0, 1.0, 0.0, gyro)
    assert v.frontLeft == pytest.approx(1.0)
    assert v.frontRight == pytest.approx(1.0)
    assert v.rearLeft == pytest.approx(1.0)
    assert v.rearRight == pytest.approx(1.0)

    # Rotate CCW
    v = MecanumDrive.driveCartesianIK(0.0, 0.0, -1.0, gyro)
    assert v.frontLeft == pytest.approx(-1.0)
    assert v.frontRight == pytest.approx(1.0)
    assert v.rearLeft == pytest.approx(-1.0)
    assert v.rearRight == pytest.approx(1.0)

    # Rotate CW
    v = MecanumDrive.driveCartesianIK(0.0, 0.0, 1.0, gyro)
    assert v.frontLeft == pytest.approx(1.0)
    assert v.frontRight == pytest.approx(-1.0)
    assert v.rearLeft == pytest.approx(1.0)
    assert v.rearRight == pytest.approx(-1.0)


def test_cartesian(wpilib_state):
    fl = MockMotorController()
    rl = MockMotorController()
    fr = MockMotorController()
    rr = MockMotorController()
    drive = MecanumDrive(
        fl.setThrottle,
        rl.setThrottle,
        fr.setThrottle,
        rr.setThrottle,
    )
    drive.setDeadband(0.0)

    # Forward
    drive.driveCartesian(1.0, 0.0, 0.0)
    assert fl.getThrottle() == pytest.approx(1.0)
    assert fr.getThrottle() == pytest.approx(1.0)
    assert rl.getThrottle() == pytest.approx(1.0)
    assert rr.getThrottle() == pytest.approx(1.0)

    # Left
    drive.driveCartesian(0.0, -1.0, 0.0)
    assert fl.getThrottle() == pytest.approx(-1.0)
    assert fr.getThrottle() == pytest.approx(1.0)
    assert rl.getThrottle() == pytest.approx(1.0)
    assert rr.getThrottle() == pytest.approx(-1.0)

    # Right
    drive.driveCartesian(0.0, 1.0, 0.0)
    assert fl.getThrottle() == pytest.approx(1.0)
    assert fr.getThrottle() == pytest.approx(-1.0)
    assert rl.getThrottle() == pytest.approx(-1.0)
    assert rr.getThrottle() == pytest.approx(1.0)

    # Rotate CCW
    drive.driveCartesian(0.0, 0.0, -1.0)
    assert fl.getThrottle() == pytest.approx(-1.0)
    assert fr.getThrottle() == pytest.approx(1.0)
    assert rl.getThrottle() == pytest.approx(-1.0)
    assert rr.getThrottle() == pytest.approx(1.0)

    # Rotate CW
    drive.driveCartesian(0.0, 0.0, 1.0)
    assert fl.getThrottle() == pytest.approx(1.0)
    assert fr.getThrottle() == pytest.approx(-1.0)
    assert rl.getThrottle() == pytest.approx(1.0)
    assert rr.getThrottle() == pytest.approx(-1.0)


def test_cartesian_gyro_90_cw(wpilib_state):
    fl = MockMotorController()
    rl = MockMotorController()
    fr = MockMotorController()
    rr = MockMotorController()
    drive = MecanumDrive(
        fl.setThrottle,
        rl.setThrottle,
        fr.setThrottle,
        rr.setThrottle,
    )
    drive.setDeadband(0.0)

    gyro = Rotation2d.fromDegrees(90)

    # Forward in global frame; left in robot frame
    drive.driveCartesian(1.0, 0.0, 0.0, gyro)
    assert fl.getThrottle() == pytest.approx(-1.0)
    assert fr.getThrottle() == pytest.approx(1.0)
    assert rl.getThrottle() == pytest.approx(1.0)
    assert rr.getThrottle() == pytest.approx(-1.0)

    # Left in global frame; backward in robot frame
    drive.driveCartesian(0.0, -1.0, 0.0, gyro)
    assert fl.getThrottle() == pytest.approx(-1.0)
    assert fr.getThrottle() == pytest.approx(-1.0)
    assert rl.getThrottle() == pytest.approx(-1.0)
    assert rr.getThrottle() == pytest.approx(-1.0)

    # Right in global frame; forward in robot frame
    drive.driveCartesian(0.0, 1.0, 0.0, gyro)
    assert fl.getThrottle() == pytest.approx(1.0)
    assert fr.getThrottle() == pytest.approx(1.0)
    assert rl.getThrottle() == pytest.approx(1.0)
    assert rr.getThrottle() == pytest.approx(1.0)

    # Rotate CCW
    drive.driveCartesian(0.0, 0.0, -1.0, gyro)
    assert fl.getThrottle() == pytest.approx(-1.0)
    assert fr.getThrottle() == pytest.approx(1.0)
    assert rl.getThrottle() == pytest.approx(-1.0)
    assert rr.getThrottle() == pytest.approx(1.0)

    # Rotate CW
    drive.driveCartesian(0.0, 0.0, 1.0, gyro)
    assert fl.getThrottle() == pytest.approx(1.0)
    assert fr.getThrottle() == pytest.approx(-1.0)
    assert rl.getThrottle() == pytest.approx(1.0)
    assert rr.getThrottle() == pytest.approx(-1.0)


def test_polar(wpilib_state):
    fl = MockMotorController()
    rl = MockMotorController()
    fr = MockMotorController()
    rr = MockMotorController()
    drive = MecanumDrive(
        fl.setThrottle,
        rl.setThrottle,
        fr.setThrottle,
        rr.setThrottle,
    )
    drive.setDeadband(0.0)

    # Forward
    drive.drivePolar(1.0, Rotation2d.fromDegrees(0), 0.0)
    assert fl.getThrottle() == pytest.approx(1.0)
    assert fr.getThrottle() == pytest.approx(1.0)
    assert rl.getThrottle() == pytest.approx(1.0)
    assert rr.getThrottle() == pytest.approx(1.0)

    # Left
    drive.drivePolar(1.0, Rotation2d.fromDegrees(-90), 0.0)
    assert fl.getThrottle() == pytest.approx(-1.0)
    assert fr.getThrottle() == pytest.approx(1.0)
    assert rl.getThrottle() == pytest.approx(1.0)
    assert rr.getThrottle() == pytest.approx(-1.0)

    # Right
    drive.drivePolar(1.0, Rotation2d.fromDegrees(90), 0.0)
    assert fl.getThrottle() == pytest.approx(1.0)
    assert fr.getThrottle() == pytest.approx(-1.0)
    assert rl.getThrottle() == pytest.approx(-1.0)
    assert rr.getThrottle() == pytest.approx(1.0)

    # Rotate CCW
    drive.drivePolar(0.0, Rotation2d.fromDegrees(0), -1.0)
    assert fl.getThrottle() == pytest.approx(-1.0)
    assert fr.getThrottle() == pytest.approx(1.0)
    assert rl.getThrottle() == pytest.approx(-1.0)
    assert rr.getThrottle() == pytest.approx(1.0)

    # Rotate CW
    drive.drivePolar(0.0, Rotation2d.fromDegrees(0), 1.0)
    assert fl.getThrottle() == pytest.approx(1.0)
    assert fr.getThrottle() == pytest.approx(-1.0)
    assert rl.getThrottle() == pytest.approx(1.0)
    assert rr.getThrottle() == pytest.approx(-1.0)
