import math

import pytest
from wpimath import Rotation2d

from wpilib import MecanumDrive


def _make_drive():
    outputs = {"fl": 0.0, "rl": 0.0, "fr": 0.0, "rr": 0.0}
    drive = MecanumDrive(
        lambda x: outputs.__setitem__("fl", x),
        lambda x: outputs.__setitem__("rl", x),
        lambda x: outputs.__setitem__("fr", x),
        lambda x: outputs.__setitem__("rr", x),
    )
    drive.setDeadband(0.0)
    return drive, outputs


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
    drive, outputs = _make_drive()

    # Forward
    drive.driveCartesian(1.0, 0.0, 0.0)
    assert outputs["fl"] == pytest.approx(1.0)
    assert outputs["fr"] == pytest.approx(1.0)
    assert outputs["rl"] == pytest.approx(1.0)
    assert outputs["rr"] == pytest.approx(1.0)

    # Left
    drive.driveCartesian(0.0, -1.0, 0.0)
    assert outputs["fl"] == pytest.approx(-1.0)
    assert outputs["fr"] == pytest.approx(1.0)
    assert outputs["rl"] == pytest.approx(1.0)
    assert outputs["rr"] == pytest.approx(-1.0)

    # Right
    drive.driveCartesian(0.0, 1.0, 0.0)
    assert outputs["fl"] == pytest.approx(1.0)
    assert outputs["fr"] == pytest.approx(-1.0)
    assert outputs["rl"] == pytest.approx(-1.0)
    assert outputs["rr"] == pytest.approx(1.0)

    # Rotate CCW
    drive.driveCartesian(0.0, 0.0, -1.0)
    assert outputs["fl"] == pytest.approx(-1.0)
    assert outputs["fr"] == pytest.approx(1.0)
    assert outputs["rl"] == pytest.approx(-1.0)
    assert outputs["rr"] == pytest.approx(1.0)

    # Rotate CW
    drive.driveCartesian(0.0, 0.0, 1.0)
    assert outputs["fl"] == pytest.approx(1.0)
    assert outputs["fr"] == pytest.approx(-1.0)
    assert outputs["rl"] == pytest.approx(1.0)
    assert outputs["rr"] == pytest.approx(-1.0)


def test_cartesian_gyro_90_cw(wpilib_state):
    drive, outputs = _make_drive()
    gyro = Rotation2d.fromDegrees(90)

    # Forward in global frame; left in robot frame
    drive.driveCartesian(1.0, 0.0, 0.0, gyro)
    assert outputs["fl"] == pytest.approx(-1.0)
    assert outputs["fr"] == pytest.approx(1.0)
    assert outputs["rl"] == pytest.approx(1.0)
    assert outputs["rr"] == pytest.approx(-1.0)

    # Left in global frame; backward in robot frame
    drive.driveCartesian(0.0, -1.0, 0.0, gyro)
    assert outputs["fl"] == pytest.approx(-1.0)
    assert outputs["fr"] == pytest.approx(-1.0)
    assert outputs["rl"] == pytest.approx(-1.0)
    assert outputs["rr"] == pytest.approx(-1.0)

    # Right in global frame; forward in robot frame
    drive.driveCartesian(0.0, 1.0, 0.0, gyro)
    assert outputs["fl"] == pytest.approx(1.0)
    assert outputs["fr"] == pytest.approx(1.0)
    assert outputs["rl"] == pytest.approx(1.0)
    assert outputs["rr"] == pytest.approx(1.0)

    # Rotate CCW
    drive.driveCartesian(0.0, 0.0, -1.0, gyro)
    assert outputs["fl"] == pytest.approx(-1.0)
    assert outputs["fr"] == pytest.approx(1.0)
    assert outputs["rl"] == pytest.approx(-1.0)
    assert outputs["rr"] == pytest.approx(1.0)

    # Rotate CW
    drive.driveCartesian(0.0, 0.0, 1.0, gyro)
    assert outputs["fl"] == pytest.approx(1.0)
    assert outputs["fr"] == pytest.approx(-1.0)
    assert outputs["rl"] == pytest.approx(1.0)
    assert outputs["rr"] == pytest.approx(-1.0)


def test_polar(wpilib_state):
    drive, outputs = _make_drive()

    # Forward
    drive.drivePolar(1.0, Rotation2d.fromDegrees(0), 0.0)
    assert outputs["fl"] == pytest.approx(1.0)
    assert outputs["fr"] == pytest.approx(1.0)
    assert outputs["rl"] == pytest.approx(1.0)
    assert outputs["rr"] == pytest.approx(1.0)

    # Left
    drive.drivePolar(1.0, Rotation2d.fromDegrees(-90), 0.0)
    assert outputs["fl"] == pytest.approx(-1.0)
    assert outputs["fr"] == pytest.approx(1.0)
    assert outputs["rl"] == pytest.approx(1.0)
    assert outputs["rr"] == pytest.approx(-1.0)

    # Right
    drive.drivePolar(1.0, Rotation2d.fromDegrees(90), 0.0)
    assert outputs["fl"] == pytest.approx(1.0)
    assert outputs["fr"] == pytest.approx(-1.0)
    assert outputs["rl"] == pytest.approx(-1.0)
    assert outputs["rr"] == pytest.approx(1.0)

    # Rotate CCW
    drive.drivePolar(0.0, Rotation2d.fromDegrees(0), -1.0)
    assert outputs["fl"] == pytest.approx(-1.0)
    assert outputs["fr"] == pytest.approx(1.0)
    assert outputs["rl"] == pytest.approx(-1.0)
    assert outputs["rr"] == pytest.approx(1.0)

    # Rotate CW
    drive.drivePolar(0.0, Rotation2d.fromDegrees(0), 1.0)
    assert outputs["fl"] == pytest.approx(1.0)
    assert outputs["fr"] == pytest.approx(-1.0)
    assert outputs["rl"] == pytest.approx(1.0)
    assert outputs["rr"] == pytest.approx(-1.0)
