import pytest

from wpilib import DifferentialDrive


def _make_drive():
    outputs = {"left": 0.0, "right": 0.0}
    drive = DifferentialDrive(
        lambda x: outputs.__setitem__("left", x),
        lambda x: outputs.__setitem__("right", x),
    )
    drive.setDeadband(0.0)
    return drive, outputs


def test_arcade_drive_ik():
    # Forward
    v = DifferentialDrive.arcadeDriveIK(1.0, 0.0, False)
    assert v.left == pytest.approx(1.0)
    assert v.right == pytest.approx(1.0)

    # Forward left turn
    v = DifferentialDrive.arcadeDriveIK(0.5, 0.5, False)
    assert v.left == pytest.approx(0.0)
    assert v.right == pytest.approx(0.5)

    # Forward right turn
    v = DifferentialDrive.arcadeDriveIK(0.5, -0.5, False)
    assert v.left == pytest.approx(0.5)
    assert v.right == pytest.approx(0.0)

    # Backward
    v = DifferentialDrive.arcadeDriveIK(-1.0, 0.0, False)
    assert v.left == pytest.approx(-1.0)
    assert v.right == pytest.approx(-1.0)

    # Backward left turn
    v = DifferentialDrive.arcadeDriveIK(-0.5, 0.5, False)
    assert v.left == pytest.approx(-0.5)
    assert v.right == pytest.approx(0.0)

    # Backward right turn
    v = DifferentialDrive.arcadeDriveIK(-0.5, -0.5, False)
    assert v.left == pytest.approx(0.0)
    assert v.right == pytest.approx(-0.5)

    # Left turn (negative sign xSpeed)
    v = DifferentialDrive.arcadeDriveIK(-0.0, 1.0, False)
    assert v.left == pytest.approx(-1.0)
    assert v.right == pytest.approx(1.0)

    # Left turn (positive sign xSpeed)
    v = DifferentialDrive.arcadeDriveIK(0.0, 1.0, False)
    assert v.left == pytest.approx(-1.0)
    assert v.right == pytest.approx(1.0)

    # Right turn (negative sign xSpeed)
    v = DifferentialDrive.arcadeDriveIK(-0.0, -1.0, False)
    assert v.left == pytest.approx(1.0)
    assert v.right == pytest.approx(-1.0)

    # Right turn (positive sign xSpeed)
    v = DifferentialDrive.arcadeDriveIK(0.0, -1.0, False)
    assert v.left == pytest.approx(1.0)
    assert v.right == pytest.approx(-1.0)


def test_arcade_drive_ik_squared():
    # Forward
    v = DifferentialDrive.arcadeDriveIK(1.0, 0.0, True)
    assert v.left == pytest.approx(1.0)
    assert v.right == pytest.approx(1.0)

    # Forward left turn
    v = DifferentialDrive.arcadeDriveIK(0.5, 0.5, True)
    assert v.left == pytest.approx(0.0)
    assert v.right == pytest.approx(0.25)

    # Forward right turn
    v = DifferentialDrive.arcadeDriveIK(0.5, -0.5, True)
    assert v.left == pytest.approx(0.25)
    assert v.right == pytest.approx(0.0)

    # Backward
    v = DifferentialDrive.arcadeDriveIK(-1.0, 0.0, True)
    assert v.left == pytest.approx(-1.0)
    assert v.right == pytest.approx(-1.0)

    # Backward left turn
    v = DifferentialDrive.arcadeDriveIK(-0.5, 0.5, True)
    assert v.left == pytest.approx(-0.25)
    assert v.right == pytest.approx(0.0)

    # Backward right turn
    v = DifferentialDrive.arcadeDriveIK(-0.5, -0.5, True)
    assert v.left == pytest.approx(0.0)
    assert v.right == pytest.approx(-0.25)

    # Rotation-only cases same as unsquared
    v = DifferentialDrive.arcadeDriveIK(-0.0, 1.0, False)
    assert v.left == pytest.approx(-1.0)
    assert v.right == pytest.approx(1.0)

    v = DifferentialDrive.arcadeDriveIK(0.0, 1.0, False)
    assert v.left == pytest.approx(-1.0)
    assert v.right == pytest.approx(1.0)

    v = DifferentialDrive.arcadeDriveIK(-0.0, -1.0, False)
    assert v.left == pytest.approx(1.0)
    assert v.right == pytest.approx(-1.0)

    v = DifferentialDrive.arcadeDriveIK(0.0, -1.0, False)
    assert v.left == pytest.approx(1.0)
    assert v.right == pytest.approx(-1.0)


def test_curvature_drive_ik():
    # Forward
    v = DifferentialDrive.curvatureDriveIK(1.0, 0.0, False)
    assert v.left == pytest.approx(1.0)
    assert v.right == pytest.approx(1.0)

    # Forward left turn
    v = DifferentialDrive.curvatureDriveIK(0.5, 0.5, False)
    assert v.left == pytest.approx(0.25)
    assert v.right == pytest.approx(0.75)

    # Forward right turn
    v = DifferentialDrive.curvatureDriveIK(0.5, -0.5, False)
    assert v.left == pytest.approx(0.75)
    assert v.right == pytest.approx(0.25)

    # Backward
    v = DifferentialDrive.curvatureDriveIK(-1.0, 0.0, False)
    assert v.left == pytest.approx(-1.0)
    assert v.right == pytest.approx(-1.0)

    # Backward left turn
    v = DifferentialDrive.curvatureDriveIK(-0.5, 0.5, False)
    assert v.left == pytest.approx(-0.75)
    assert v.right == pytest.approx(-0.25)

    # Backward right turn
    v = DifferentialDrive.curvatureDriveIK(-0.5, -0.5, False)
    assert v.left == pytest.approx(-0.25)
    assert v.right == pytest.approx(-0.75)


def test_curvature_drive_ik_turn_in_place():
    # Forward
    v = DifferentialDrive.curvatureDriveIK(1.0, 0.0, True)
    assert v.left == pytest.approx(1.0)
    assert v.right == pytest.approx(1.0)

    # Forward left turn
    v = DifferentialDrive.curvatureDriveIK(0.5, 0.5, True)
    assert v.left == pytest.approx(0.0)
    assert v.right == pytest.approx(1.0)

    # Forward right turn
    v = DifferentialDrive.curvatureDriveIK(0.5, -0.5, True)
    assert v.left == pytest.approx(1.0)
    assert v.right == pytest.approx(0.0)

    # Backward
    v = DifferentialDrive.curvatureDriveIK(-1.0, 0.0, True)
    assert v.left == pytest.approx(-1.0)
    assert v.right == pytest.approx(-1.0)

    # Backward left turn
    v = DifferentialDrive.curvatureDriveIK(-0.5, 0.5, True)
    assert v.left == pytest.approx(-1.0)
    assert v.right == pytest.approx(0.0)

    # Backward right turn
    v = DifferentialDrive.curvatureDriveIK(-0.5, -0.5, True)
    assert v.left == pytest.approx(0.0)
    assert v.right == pytest.approx(-1.0)


def test_tank_drive_ik():
    v = DifferentialDrive.tankDriveIK(1.0, 1.0, False)
    assert v.left == pytest.approx(1.0)
    assert v.right == pytest.approx(1.0)

    v = DifferentialDrive.tankDriveIK(0.5, 1.0, False)
    assert v.left == pytest.approx(0.5)
    assert v.right == pytest.approx(1.0)

    v = DifferentialDrive.tankDriveIK(1.0, 0.5, False)
    assert v.left == pytest.approx(1.0)
    assert v.right == pytest.approx(0.5)

    v = DifferentialDrive.tankDriveIK(-1.0, -1.0, False)
    assert v.left == pytest.approx(-1.0)
    assert v.right == pytest.approx(-1.0)

    v = DifferentialDrive.tankDriveIK(-0.5, -1.0, False)
    assert v.left == pytest.approx(-0.5)
    assert v.right == pytest.approx(-1.0)

    v = DifferentialDrive.tankDriveIK(-0.5, 1.0, False)
    assert v.left == pytest.approx(-0.5)
    assert v.right == pytest.approx(1.0)


def test_tank_drive_ik_squared():
    v = DifferentialDrive.tankDriveIK(1.0, 1.0, True)
    assert v.left == pytest.approx(1.0)
    assert v.right == pytest.approx(1.0)

    v = DifferentialDrive.tankDriveIK(0.5, 1.0, True)
    assert v.left == pytest.approx(0.25)
    assert v.right == pytest.approx(1.0)

    v = DifferentialDrive.tankDriveIK(1.0, 0.5, True)
    assert v.left == pytest.approx(1.0)
    assert v.right == pytest.approx(0.25)

    v = DifferentialDrive.tankDriveIK(-1.0, -1.0, True)
    assert v.left == pytest.approx(-1.0)
    assert v.right == pytest.approx(-1.0)

    v = DifferentialDrive.tankDriveIK(-0.5, -1.0, True)
    assert v.left == pytest.approx(-0.25)
    assert v.right == pytest.approx(-1.0)

    v = DifferentialDrive.tankDriveIK(-1.0, -0.5, True)
    assert v.left == pytest.approx(-1.0)
    assert v.right == pytest.approx(-0.25)


def test_arcade_drive(wpilib_state):
    drive, outputs = _make_drive()

    drive.arcadeDrive(1.0, 0.0, False)
    assert outputs["left"] == pytest.approx(1.0)
    assert outputs["right"] == pytest.approx(1.0)

    drive.arcadeDrive(0.5, 0.5, False)
    assert outputs["left"] == pytest.approx(0.0)
    assert outputs["right"] == pytest.approx(0.5)

    drive.arcadeDrive(0.5, -0.5, False)
    assert outputs["left"] == pytest.approx(0.5)
    assert outputs["right"] == pytest.approx(0.0)

    drive.arcadeDrive(-1.0, 0.0, False)
    assert outputs["left"] == pytest.approx(-1.0)
    assert outputs["right"] == pytest.approx(-1.0)

    drive.arcadeDrive(-0.5, 0.5, False)
    assert outputs["left"] == pytest.approx(-0.5)
    assert outputs["right"] == pytest.approx(0.0)

    drive.arcadeDrive(-0.5, -0.5, False)
    assert outputs["left"] == pytest.approx(0.0)
    assert outputs["right"] == pytest.approx(-0.5)


def test_arcade_drive_squared(wpilib_state):
    drive, outputs = _make_drive()

    drive.arcadeDrive(1.0, 0.0, True)
    assert outputs["left"] == pytest.approx(1.0)
    assert outputs["right"] == pytest.approx(1.0)

    drive.arcadeDrive(0.5, 0.5, True)
    assert outputs["left"] == pytest.approx(0.0)
    assert outputs["right"] == pytest.approx(0.25)

    drive.arcadeDrive(0.5, -0.5, True)
    assert outputs["left"] == pytest.approx(0.25)
    assert outputs["right"] == pytest.approx(0.0)

    drive.arcadeDrive(-1.0, 0.0, True)
    assert outputs["left"] == pytest.approx(-1.0)
    assert outputs["right"] == pytest.approx(-1.0)

    drive.arcadeDrive(-0.5, 0.5, True)
    assert outputs["left"] == pytest.approx(-0.25)
    assert outputs["right"] == pytest.approx(0.0)

    drive.arcadeDrive(-0.5, -0.5, True)
    assert outputs["left"] == pytest.approx(0.0)
    assert outputs["right"] == pytest.approx(-0.25)


def test_curvature_drive(wpilib_state):
    drive, outputs = _make_drive()

    drive.curvatureDrive(1.0, 0.0, False)
    assert outputs["left"] == pytest.approx(1.0)
    assert outputs["right"] == pytest.approx(1.0)

    drive.curvatureDrive(0.5, 0.5, False)
    assert outputs["left"] == pytest.approx(0.25)
    assert outputs["right"] == pytest.approx(0.75)

    drive.curvatureDrive(0.5, -0.5, False)
    assert outputs["left"] == pytest.approx(0.75)
    assert outputs["right"] == pytest.approx(0.25)

    drive.curvatureDrive(-1.0, 0.0, False)
    assert outputs["left"] == pytest.approx(-1.0)
    assert outputs["right"] == pytest.approx(-1.0)

    drive.curvatureDrive(-0.5, 0.5, False)
    assert outputs["left"] == pytest.approx(-0.75)
    assert outputs["right"] == pytest.approx(-0.25)

    drive.curvatureDrive(-0.5, -0.5, False)
    assert outputs["left"] == pytest.approx(-0.25)
    assert outputs["right"] == pytest.approx(-0.75)


def test_curvature_drive_turn_in_place(wpilib_state):
    drive, outputs = _make_drive()

    drive.curvatureDrive(1.0, 0.0, True)
    assert outputs["left"] == pytest.approx(1.0)
    assert outputs["right"] == pytest.approx(1.0)

    drive.curvatureDrive(0.5, 0.5, True)
    assert outputs["left"] == pytest.approx(0.0)
    assert outputs["right"] == pytest.approx(1.0)

    drive.curvatureDrive(0.5, -0.5, True)
    assert outputs["left"] == pytest.approx(1.0)
    assert outputs["right"] == pytest.approx(0.0)

    drive.curvatureDrive(-1.0, 0.0, True)
    assert outputs["left"] == pytest.approx(-1.0)
    assert outputs["right"] == pytest.approx(-1.0)

    drive.curvatureDrive(-0.5, 0.5, True)
    assert outputs["left"] == pytest.approx(-1.0)
    assert outputs["right"] == pytest.approx(0.0)

    drive.curvatureDrive(-0.5, -0.5, True)
    assert outputs["left"] == pytest.approx(0.0)
    assert outputs["right"] == pytest.approx(-1.0)


def test_tank_drive(wpilib_state):
    drive, outputs = _make_drive()

    drive.tankDrive(1.0, 1.0, False)
    assert outputs["left"] == pytest.approx(1.0)
    assert outputs["right"] == pytest.approx(1.0)

    drive.tankDrive(0.5, 1.0, False)
    assert outputs["left"] == pytest.approx(0.5)
    assert outputs["right"] == pytest.approx(1.0)

    drive.tankDrive(1.0, 0.5, False)
    assert outputs["left"] == pytest.approx(1.0)
    assert outputs["right"] == pytest.approx(0.5)

    drive.tankDrive(-1.0, -1.0, False)
    assert outputs["left"] == pytest.approx(-1.0)
    assert outputs["right"] == pytest.approx(-1.0)

    drive.tankDrive(-0.5, -1.0, False)
    assert outputs["left"] == pytest.approx(-0.5)
    assert outputs["right"] == pytest.approx(-1.0)

    drive.tankDrive(-0.5, 1.0, False)
    assert outputs["left"] == pytest.approx(-0.5)
    assert outputs["right"] == pytest.approx(1.0)


def test_tank_drive_squared(wpilib_state):
    drive, outputs = _make_drive()

    drive.tankDrive(1.0, 1.0, True)
    assert outputs["left"] == pytest.approx(1.0)
    assert outputs["right"] == pytest.approx(1.0)

    drive.tankDrive(0.5, 1.0, True)
    assert outputs["left"] == pytest.approx(0.25)
    assert outputs["right"] == pytest.approx(1.0)

    drive.tankDrive(1.0, 0.5, True)
    assert outputs["left"] == pytest.approx(1.0)
    assert outputs["right"] == pytest.approx(0.25)

    drive.tankDrive(-1.0, -1.0, True)
    assert outputs["left"] == pytest.approx(-1.0)
    assert outputs["right"] == pytest.approx(-1.0)

    drive.tankDrive(-0.5, -1.0, True)
    assert outputs["left"] == pytest.approx(-0.25)
    assert outputs["right"] == pytest.approx(-1.0)

    drive.tankDrive(-1.0, -0.5, True)
    assert outputs["left"] == pytest.approx(-1.0)
    assert outputs["right"] == pytest.approx(-0.25)
