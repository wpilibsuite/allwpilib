import pytest

from wpilib import DifferentialDrive


class MockMotorController:
    def __init__(self):
        self.throttle = 0

    def set_throttle(self, throttle):
        self.throttle = throttle

    def get_throttle(self):
        return self.throttle


def test_arcade_drive_ik():
    # Forward
    v = DifferentialDrive.arcade_drive_ik(1.0, 0.0, False)
    assert v.left == pytest.approx(1.0)
    assert v.right == pytest.approx(1.0)

    # Forward left turn
    v = DifferentialDrive.arcade_drive_ik(0.5, 0.5, False)
    assert v.left == pytest.approx(0.0)
    assert v.right == pytest.approx(0.5)

    # Forward right turn
    v = DifferentialDrive.arcade_drive_ik(0.5, -0.5, False)
    assert v.left == pytest.approx(0.5)
    assert v.right == pytest.approx(0.0)

    # Backward
    v = DifferentialDrive.arcade_drive_ik(-1.0, 0.0, False)
    assert v.left == pytest.approx(-1.0)
    assert v.right == pytest.approx(-1.0)

    # Backward left turn
    v = DifferentialDrive.arcade_drive_ik(-0.5, 0.5, False)
    assert v.left == pytest.approx(-0.5)
    assert v.right == pytest.approx(0.0)

    # Backward right turn
    v = DifferentialDrive.arcade_drive_ik(-0.5, -0.5, False)
    assert v.left == pytest.approx(0.0)
    assert v.right == pytest.approx(-0.5)

    # Left turn (negative sign x_speed)
    v = DifferentialDrive.arcade_drive_ik(-0.0, 1.0, False)
    assert v.left == pytest.approx(-1.0)
    assert v.right == pytest.approx(1.0)

    # Left turn (positive sign x_speed)
    v = DifferentialDrive.arcade_drive_ik(0.0, 1.0, False)
    assert v.left == pytest.approx(-1.0)
    assert v.right == pytest.approx(1.0)

    # Right turn (negative sign x_speed)
    v = DifferentialDrive.arcade_drive_ik(-0.0, -1.0, False)
    assert v.left == pytest.approx(1.0)
    assert v.right == pytest.approx(-1.0)

    # Right turn (positive sign x_speed)
    v = DifferentialDrive.arcade_drive_ik(0.0, -1.0, False)
    assert v.left == pytest.approx(1.0)
    assert v.right == pytest.approx(-1.0)


def test_arcade_drive_ik_squared():
    # Forward
    v = DifferentialDrive.arcade_drive_ik(1.0, 0.0, True)
    assert v.left == pytest.approx(1.0)
    assert v.right == pytest.approx(1.0)

    # Forward left turn
    v = DifferentialDrive.arcade_drive_ik(0.5, 0.5, True)
    assert v.left == pytest.approx(0.0)
    assert v.right == pytest.approx(0.25)

    # Forward right turn
    v = DifferentialDrive.arcade_drive_ik(0.5, -0.5, True)
    assert v.left == pytest.approx(0.25)
    assert v.right == pytest.approx(0.0)

    # Backward
    v = DifferentialDrive.arcade_drive_ik(-1.0, 0.0, True)
    assert v.left == pytest.approx(-1.0)
    assert v.right == pytest.approx(-1.0)

    # Backward left turn
    v = DifferentialDrive.arcade_drive_ik(-0.5, 0.5, True)
    assert v.left == pytest.approx(-0.25)
    assert v.right == pytest.approx(0.0)

    # Backward right turn
    v = DifferentialDrive.arcade_drive_ik(-0.5, -0.5, True)
    assert v.left == pytest.approx(0.0)
    assert v.right == pytest.approx(-0.25)

    # Rotation-only cases same as unsquared
    v = DifferentialDrive.arcade_drive_ik(-0.0, 1.0, False)
    assert v.left == pytest.approx(-1.0)
    assert v.right == pytest.approx(1.0)

    v = DifferentialDrive.arcade_drive_ik(0.0, 1.0, False)
    assert v.left == pytest.approx(-1.0)
    assert v.right == pytest.approx(1.0)

    v = DifferentialDrive.arcade_drive_ik(-0.0, -1.0, False)
    assert v.left == pytest.approx(1.0)
    assert v.right == pytest.approx(-1.0)

    v = DifferentialDrive.arcade_drive_ik(0.0, -1.0, False)
    assert v.left == pytest.approx(1.0)
    assert v.right == pytest.approx(-1.0)


def test_curvature_drive_ik():
    # Forward
    v = DifferentialDrive.curvature_drive_ik(1.0, 0.0, False)
    assert v.left == pytest.approx(1.0)
    assert v.right == pytest.approx(1.0)

    # Forward left turn
    v = DifferentialDrive.curvature_drive_ik(0.5, 0.5, False)
    assert v.left == pytest.approx(0.25)
    assert v.right == pytest.approx(0.75)

    # Forward right turn
    v = DifferentialDrive.curvature_drive_ik(0.5, -0.5, False)
    assert v.left == pytest.approx(0.75)
    assert v.right == pytest.approx(0.25)

    # Backward
    v = DifferentialDrive.curvature_drive_ik(-1.0, 0.0, False)
    assert v.left == pytest.approx(-1.0)
    assert v.right == pytest.approx(-1.0)

    # Backward left turn
    v = DifferentialDrive.curvature_drive_ik(-0.5, 0.5, False)
    assert v.left == pytest.approx(-0.75)
    assert v.right == pytest.approx(-0.25)

    # Backward right turn
    v = DifferentialDrive.curvature_drive_ik(-0.5, -0.5, False)
    assert v.left == pytest.approx(-0.25)
    assert v.right == pytest.approx(-0.75)


def test_curvature_drive_ik_turn_in_place():
    # Forward
    v = DifferentialDrive.curvature_drive_ik(1.0, 0.0, True)
    assert v.left == pytest.approx(1.0)
    assert v.right == pytest.approx(1.0)

    # Forward left turn
    v = DifferentialDrive.curvature_drive_ik(0.5, 0.5, True)
    assert v.left == pytest.approx(0.0)
    assert v.right == pytest.approx(1.0)

    # Forward right turn
    v = DifferentialDrive.curvature_drive_ik(0.5, -0.5, True)
    assert v.left == pytest.approx(1.0)
    assert v.right == pytest.approx(0.0)

    # Backward
    v = DifferentialDrive.curvature_drive_ik(-1.0, 0.0, True)
    assert v.left == pytest.approx(-1.0)
    assert v.right == pytest.approx(-1.0)

    # Backward left turn
    v = DifferentialDrive.curvature_drive_ik(-0.5, 0.5, True)
    assert v.left == pytest.approx(-1.0)
    assert v.right == pytest.approx(0.0)

    # Backward right turn
    v = DifferentialDrive.curvature_drive_ik(-0.5, -0.5, True)
    assert v.left == pytest.approx(0.0)
    assert v.right == pytest.approx(-1.0)


def test_tank_drive_ik():
    v = DifferentialDrive.tank_drive_ik(1.0, 1.0, False)
    assert v.left == pytest.approx(1.0)
    assert v.right == pytest.approx(1.0)

    v = DifferentialDrive.tank_drive_ik(0.5, 1.0, False)
    assert v.left == pytest.approx(0.5)
    assert v.right == pytest.approx(1.0)

    v = DifferentialDrive.tank_drive_ik(1.0, 0.5, False)
    assert v.left == pytest.approx(1.0)
    assert v.right == pytest.approx(0.5)

    v = DifferentialDrive.tank_drive_ik(-1.0, -1.0, False)
    assert v.left == pytest.approx(-1.0)
    assert v.right == pytest.approx(-1.0)

    v = DifferentialDrive.tank_drive_ik(-0.5, -1.0, False)
    assert v.left == pytest.approx(-0.5)
    assert v.right == pytest.approx(-1.0)

    v = DifferentialDrive.tank_drive_ik(-0.5, 1.0, False)
    assert v.left == pytest.approx(-0.5)
    assert v.right == pytest.approx(1.0)


def test_tank_drive_ik_squared():
    v = DifferentialDrive.tank_drive_ik(1.0, 1.0, True)
    assert v.left == pytest.approx(1.0)
    assert v.right == pytest.approx(1.0)

    v = DifferentialDrive.tank_drive_ik(0.5, 1.0, True)
    assert v.left == pytest.approx(0.25)
    assert v.right == pytest.approx(1.0)

    v = DifferentialDrive.tank_drive_ik(1.0, 0.5, True)
    assert v.left == pytest.approx(1.0)
    assert v.right == pytest.approx(0.25)

    v = DifferentialDrive.tank_drive_ik(-1.0, -1.0, True)
    assert v.left == pytest.approx(-1.0)
    assert v.right == pytest.approx(-1.0)

    v = DifferentialDrive.tank_drive_ik(-0.5, -1.0, True)
    assert v.left == pytest.approx(-0.25)
    assert v.right == pytest.approx(-1.0)

    v = DifferentialDrive.tank_drive_ik(-1.0, -0.5, True)
    assert v.left == pytest.approx(-1.0)
    assert v.right == pytest.approx(-0.25)


def test_arcade_drive(wpilib_state):
    left = MockMotorController()
    right = MockMotorController()
    drive = DifferentialDrive(
        left.set_throttle,
        right.set_throttle,
    )
    drive.set_deadband(0.0)

    drive.arcade_drive(1.0, 0.0, False)
    assert left.get_throttle() == pytest.approx(1.0)
    assert right.get_throttle() == pytest.approx(1.0)

    drive.arcade_drive(0.5, 0.5, False)
    assert left.get_throttle() == pytest.approx(0.0)
    assert right.get_throttle() == pytest.approx(0.5)

    drive.arcade_drive(0.5, -0.5, False)
    assert left.get_throttle() == pytest.approx(0.5)
    assert right.get_throttle() == pytest.approx(0.0)

    drive.arcade_drive(-1.0, 0.0, False)
    assert left.get_throttle() == pytest.approx(-1.0)
    assert right.get_throttle() == pytest.approx(-1.0)

    drive.arcade_drive(-0.5, 0.5, False)
    assert left.get_throttle() == pytest.approx(-0.5)
    assert right.get_throttle() == pytest.approx(0.0)

    drive.arcade_drive(-0.5, -0.5, False)
    assert left.get_throttle() == pytest.approx(0.0)
    assert right.get_throttle() == pytest.approx(-0.5)


def test_arcade_drive_squared(wpilib_state):
    left = MockMotorController()
    right = MockMotorController()
    drive = DifferentialDrive(
        left.set_throttle,
        right.set_throttle,
    )
    drive.set_deadband(0.0)

    drive.arcade_drive(1.0, 0.0, True)
    assert left.get_throttle() == pytest.approx(1.0)
    assert right.get_throttle() == pytest.approx(1.0)

    drive.arcade_drive(0.5, 0.5, True)
    assert left.get_throttle() == pytest.approx(0.0)
    assert right.get_throttle() == pytest.approx(0.25)

    drive.arcade_drive(0.5, -0.5, True)
    assert left.get_throttle() == pytest.approx(0.25)
    assert right.get_throttle() == pytest.approx(0.0)

    drive.arcade_drive(-1.0, 0.0, True)
    assert left.get_throttle() == pytest.approx(-1.0)
    assert right.get_throttle() == pytest.approx(-1.0)

    drive.arcade_drive(-0.5, 0.5, True)
    assert left.get_throttle() == pytest.approx(-0.25)
    assert right.get_throttle() == pytest.approx(0.0)

    drive.arcade_drive(-0.5, -0.5, True)
    assert left.get_throttle() == pytest.approx(0.0)
    assert right.get_throttle() == pytest.approx(-0.25)


def test_curvature_drive(wpilib_state):
    left = MockMotorController()
    right = MockMotorController()
    drive = DifferentialDrive(
        left.set_throttle,
        right.set_throttle,
    )
    drive.set_deadband(0.0)

    drive.curvature_drive(1.0, 0.0, False)
    assert left.get_throttle() == pytest.approx(1.0)
    assert right.get_throttle() == pytest.approx(1.0)

    drive.curvature_drive(0.5, 0.5, False)
    assert left.get_throttle() == pytest.approx(0.25)
    assert right.get_throttle() == pytest.approx(0.75)

    drive.curvature_drive(0.5, -0.5, False)
    assert left.get_throttle() == pytest.approx(0.75)
    assert right.get_throttle() == pytest.approx(0.25)

    drive.curvature_drive(-1.0, 0.0, False)
    assert left.get_throttle() == pytest.approx(-1.0)
    assert right.get_throttle() == pytest.approx(-1.0)

    drive.curvature_drive(-0.5, 0.5, False)
    assert left.get_throttle() == pytest.approx(-0.75)
    assert right.get_throttle() == pytest.approx(-0.25)

    drive.curvature_drive(-0.5, -0.5, False)
    assert left.get_throttle() == pytest.approx(-0.25)
    assert right.get_throttle() == pytest.approx(-0.75)


def test_curvature_drive_turn_in_place(wpilib_state):
    left = MockMotorController()
    right = MockMotorController()
    drive = DifferentialDrive(
        left.set_throttle,
        right.set_throttle,
    )
    drive.set_deadband(0.0)

    drive.curvature_drive(1.0, 0.0, True)
    assert left.get_throttle() == pytest.approx(1.0)
    assert right.get_throttle() == pytest.approx(1.0)

    drive.curvature_drive(0.5, 0.5, True)
    assert left.get_throttle() == pytest.approx(0.0)
    assert right.get_throttle() == pytest.approx(1.0)

    drive.curvature_drive(0.5, -0.5, True)
    assert left.get_throttle() == pytest.approx(1.0)
    assert right.get_throttle() == pytest.approx(0.0)

    drive.curvature_drive(-1.0, 0.0, True)
    assert left.get_throttle() == pytest.approx(-1.0)
    assert right.get_throttle() == pytest.approx(-1.0)

    drive.curvature_drive(-0.5, 0.5, True)
    assert left.get_throttle() == pytest.approx(-1.0)
    assert right.get_throttle() == pytest.approx(0.0)

    drive.curvature_drive(-0.5, -0.5, True)
    assert left.get_throttle() == pytest.approx(0.0)
    assert right.get_throttle() == pytest.approx(-1.0)


def test_tank_drive(wpilib_state):
    left = MockMotorController()
    right = MockMotorController()
    drive = DifferentialDrive(
        left.set_throttle,
        right.set_throttle,
    )
    drive.set_deadband(0.0)

    drive.tank_drive(1.0, 1.0, False)
    assert left.get_throttle() == pytest.approx(1.0)
    assert right.get_throttle() == pytest.approx(1.0)

    drive.tank_drive(0.5, 1.0, False)
    assert left.get_throttle() == pytest.approx(0.5)
    assert right.get_throttle() == pytest.approx(1.0)

    drive.tank_drive(1.0, 0.5, False)
    assert left.get_throttle() == pytest.approx(1.0)
    assert right.get_throttle() == pytest.approx(0.5)

    drive.tank_drive(-1.0, -1.0, False)
    assert left.get_throttle() == pytest.approx(-1.0)
    assert right.get_throttle() == pytest.approx(-1.0)

    drive.tank_drive(-0.5, -1.0, False)
    assert left.get_throttle() == pytest.approx(-0.5)
    assert right.get_throttle() == pytest.approx(-1.0)

    drive.tank_drive(-0.5, 1.0, False)
    assert left.get_throttle() == pytest.approx(-0.5)
    assert right.get_throttle() == pytest.approx(1.0)


def test_tank_drive_squared(wpilib_state):
    left = MockMotorController()
    right = MockMotorController()
    drive = DifferentialDrive(
        left.set_throttle,
        right.set_throttle,
    )
    drive.set_deadband(0.0)

    drive.tank_drive(1.0, 1.0, True)
    assert left.get_throttle() == pytest.approx(1.0)
    assert right.get_throttle() == pytest.approx(1.0)

    drive.tank_drive(0.5, 1.0, True)
    assert left.get_throttle() == pytest.approx(0.25)
    assert right.get_throttle() == pytest.approx(1.0)

    drive.tank_drive(1.0, 0.5, True)
    assert left.get_throttle() == pytest.approx(1.0)
    assert right.get_throttle() == pytest.approx(0.25)

    drive.tank_drive(-1.0, -1.0, True)
    assert left.get_throttle() == pytest.approx(-1.0)
    assert right.get_throttle() == pytest.approx(-1.0)

    drive.tank_drive(-0.5, -1.0, True)
    assert left.get_throttle() == pytest.approx(-0.25)
    assert right.get_throttle() == pytest.approx(-1.0)

    drive.tank_drive(-1.0, -0.5, True)
    assert left.get_throttle() == pytest.approx(-1.0)
    assert right.get_throttle() == pytest.approx(-0.25)
