import pytest

from wpimath import Quaternion, Rotation3d
from wpilib import OnboardIMU
from wpilib.simulation import OnboardIMUSim


@pytest.fixture(autouse=True)
def reset_imu():
    sim = OnboardIMUSim()

    def reset():
        for axis in "xyz":
            getattr(sim, f"set_angle_{axis}")(0)
            getattr(sim, f"set_gyro_rate_{axis}")(0)
            getattr(sim, f"set_accel_{axis}")(0)
        sim.set_yaw(0)

    reset()
    yield
    reset()


def test_sim_device() -> None:

    imu = OnboardIMU(OnboardIMU.MountOrientation.FLAT)
    sim = OnboardIMUSim()
    assert imu.get_quaternion() == Quaternion()

    assert 0.0 == imu.get_angle_x()
    assert 0.0 == imu.get_angle_y()
    assert 0.0 == imu.get_angle_z()
    assert 0.0 == imu.get_gyro_rate_x()
    assert 0.0 == imu.get_gyro_rate_y()
    assert 0.0 == imu.get_gyro_rate_z()
    assert 0.0 == imu.get_accel_x()
    assert 0.0 == imu.get_accel_y()
    assert 0.0 == imu.get_accel_z()

    sim.set_angle_x(1)
    sim.set_angle_y(2)
    sim.set_angle_z(3)

    sim.set_gyro_rate_x(3.504)
    sim.set_gyro_rate_y(1.91)
    sim.set_gyro_rate_z(22.9)

    sim.set_accel_x(-1)
    sim.set_accel_y(-2)
    sim.set_accel_z(-3)

    assert 1.0 == imu.get_angle_x()
    assert 2.0 == imu.get_angle_y()
    assert 3.0 == imu.get_angle_z()

    assert 3.504 == imu.get_gyro_rate_x()
    assert 1.91 == imu.get_gyro_rate_y()
    assert 22.9 == imu.get_gyro_rate_z()

    assert -1.0 == imu.get_accel_x()
    assert -2.0 == imu.get_accel_y()
    assert -3.0 == imu.get_accel_z()

    sim.set_yaw(1.234)
    rotation = Rotation3d(1, 2, 3)
    assert imu.get_rotation3d() == rotation
    imu.reset_yaw()
    assert imu.get_yaw() == 0
    sim.set_yaw(2)
    assert imu.get_rotation2d().radians() == pytest.approx(0.766)
    assert imu.get_angle_z() == 3
    assert imu.get_rotation3d() == rotation
