import pytest

from wpilib import DifferentialDrive

class MockMotorController:
    def __init__(self):
        self.throttle = 0

    def setThrottle(self, throttle):
        self.throttle = throttle

    def getThrottle(self):
        return self.throttle


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
    left = MockMotorController()
    right = MockMotorController()
    drive = DifferentialDrive(
        left.setThrottle,
        right.setThrottle,
    )
    drive.setDeadband(0.0)

    drive.arcadeDrive(1.0, 0.0, False)
    assert left.getThrottle() == pytest.approx(1.0)
    assert right.getThrottle() == pytest.approx(1.0)

    drive.arcadeDrive(0.5, 0.5, False)
    assert left.getThrottle() == pytest.approx(0.0)
    assert right.getThrottle() == pytest.approx(0.5)

    drive.arcadeDrive(0.5, -0.5, False)
    assert left.getThrottle() == pytest.approx(0.5)
    assert right.getThrottle() == pytest.approx(0.0)

    drive.arcadeDrive(-1.0, 0.0, False)
    assert left.getThrottle() == pytest.approx(-1.0)
    assert right.getThrottle() == pytest.approx(-1.0)

    drive.arcadeDrive(-0.5, 0.5, False)
    assert left.getThrottle() == pytest.approx(-0.5)
    assert right.getThrottle() == pytest.approx(0.0)

    drive.arcadeDrive(-0.5, -0.5, False)
    assert left.getThrottle() == pytest.approx(0.0)
    assert right.getThrottle() == pytest.approx(-0.5)


def test_arcade_drive_squared(wpilib_state):
    left = MockMotorController()
    right = MockMotorController()
    drive = DifferentialDrive(
        left.setThrottle,
        right.setThrottle,
    )
    drive.setDeadband(0.0)

    drive.arcadeDrive(1.0, 0.0, True)
    assert left.getThrottle() == pytest.approx(1.0)
    assert right.getThrottle() == pytest.approx(1.0)

    drive.arcadeDrive(0.5, 0.5, True)
    assert left.getThrottle() == pytest.approx(0.0)
    assert right.getThrottle() == pytest.approx(0.25)

    drive.arcadeDrive(0.5, -0.5, True)
    assert left.getThrottle() == pytest.approx(0.25)
    assert right.getThrottle() == pytest.approx(0.0)

    drive.arcadeDrive(-1.0, 0.0, True)
    assert left.getThrottle() == pytest.approx(-1.0)
    assert right.getThrottle() == pytest.approx(-1.0)

    drive.arcadeDrive(-0.5, 0.5, True)
    assert left.getThrottle() == pytest.approx(-0.25)
    assert right.getThrottle() == pytest.approx(0.0)

    drive.arcadeDrive(-0.5, -0.5, True)
    assert left.getThrottle() == pytest.approx(0.0)
    assert right.getThrottle() == pytest.approx(-0.25)


def test_curvature_drive(wpilib_state):
    left = MockMotorController()
    right = MockMotorController()
    drive = DifferentialDrive(
        left.setThrottle,
        right.setThrottle,
    )
    drive.setDeadband(0.0)

    drive.curvatureDrive(1.0, 0.0, False)
    assert left.getThrottle() == pytest.approx(1.0)
    assert right.getThrottle() == pytest.approx(1.0)

    drive.curvatureDrive(0.5, 0.5, False)
    assert left.getThrottle() == pytest.approx(0.25)
    assert right.getThrottle() == pytest.approx(0.75)

    drive.curvatureDrive(0.5, -0.5, False)
    assert left.getThrottle() == pytest.approx(0.75)
    assert right.getThrottle() == pytest.approx(0.25)

    drive.curvatureDrive(-1.0, 0.0, False)
    assert left.getThrottle() == pytest.approx(-1.0)
    assert right.getThrottle() == pytest.approx(-1.0)

    drive.curvatureDrive(-0.5, 0.5, False)
    assert left.getThrottle() == pytest.approx(-0.75)
    assert right.getThrottle() == pytest.approx(-0.25)

    drive.curvatureDrive(-0.5, -0.5, False)
    assert left.getThrottle() == pytest.approx(-0.25)
    assert right.getThrottle() == pytest.approx(-0.75)


def test_curvature_drive_turn_in_place(wpilib_state):
    left = MockMotorController()
    right = MockMotorController()
    drive = DifferentialDrive(
        left.setThrottle,
        right.setThrottle,
    )
    drive.setDeadband(0.0)

    drive.curvatureDrive(1.0, 0.0, True)
    assert left.getThrottle() == pytest.approx(1.0)
    assert right.getThrottle() == pytest.approx(1.0)

    drive.curvatureDrive(0.5, 0.5, True)
    assert left.getThrottle() == pytest.approx(0.0)
    assert right.getThrottle() == pytest.approx(1.0)

    drive.curvatureDrive(0.5, -0.5, True)
    assert left.getThrottle() == pytest.approx(1.0)
    assert right.getThrottle() == pytest.approx(0.0)

    drive.curvatureDrive(-1.0, 0.0, True)
    assert left.getThrottle() == pytest.approx(-1.0)
    assert right.getThrottle() == pytest.approx(-1.0)

    drive.curvatureDrive(-0.5, 0.5, True)
    assert left.getThrottle() == pytest.approx(-1.0)
    assert right.getThrottle() == pytest.approx(0.0)

    drive.curvatureDrive(-0.5, -0.5, True)
    assert left.getThrottle() == pytest.approx(0.0)
    assert right.getThrottle() == pytest.approx(-1.0)


def test_tank_drive(wpilib_state):
    left = MockMotorController()
    right = MockMotorController()
    drive = DifferentialDrive(
        left.setThrottle,
        right.setThrottle,
    )
    drive.setDeadband(0.0)

    drive.tankDrive(1.0, 1.0, False)
    assert left.getThrottle() == pytest.approx(1.0)
    assert right.getThrottle() == pytest.approx(1.0)

    drive.tankDrive(0.5, 1.0, False)
    assert left.getThrottle() == pytest.approx(0.5)
    assert right.getThrottle() == pytest.approx(1.0)

    drive.tankDrive(1.0, 0.5, False)
    assert left.getThrottle() == pytest.approx(1.0)
    assert right.getThrottle() == pytest.approx(0.5)

    drive.tankDrive(-1.0, -1.0, False)
    assert left.getThrottle() == pytest.approx(-1.0)
    assert right.getThrottle() == pytest.approx(-1.0)

    drive.tankDrive(-0.5, -1.0, False)
    assert left.getThrottle() == pytest.approx(-0.5)
    assert right.getThrottle() == pytest.approx(-1.0)

    drive.tankDrive(-0.5, 1.0, False)
    assert left.getThrottle() == pytest.approx(-0.5)
    assert right.getThrottle() == pytest.approx(1.0)


def test_tank_drive_squared(wpilib_state):
    left = MockMotorController()
    right = MockMotorController()
    drive = DifferentialDrive(
        left.setThrottle,
        right.setThrottle,
    )
    drive.setDeadband(0.0)

    drive.tankDrive(1.0, 1.0, True)
    assert left.getThrottle() == pytest.approx(1.0)
    assert right.getThrottle() == pytest.approx(1.0)

    drive.tankDrive(0.5, 1.0, True)
    assert left.getThrottle() == pytest.approx(0.25)
    assert right.getThrottle() == pytest.approx(1.0)

    drive.tankDrive(1.0, 0.5, True)
    assert left.getThrottle() == pytest.approx(1.0)
    assert right.getThrottle() == pytest.approx(0.25)

    drive.tankDrive(-1.0, -1.0, True)
    assert left.getThrottle() == pytest.approx(-1.0)
    assert right.getThrottle() == pytest.approx(-1.0)

    drive.tankDrive(-0.5, -1.0, True)
    assert left.getThrottle() == pytest.approx(-0.25)
    assert right.getThrottle() == pytest.approx(-1.0)

    drive.tankDrive(-1.0, -0.5, True)
    assert left.getThrottle() == pytest.approx(-1.0)
    assert right.getThrottle() == pytest.approx(-0.25)
