from wpilib import OnboardIMU
from wpilib.simulation import OnboardIMUSim


def test_sim_device() -> None:

    imu = OnboardIMU(OnboardIMU.MountOrientation.FLAT)
    sim = OnboardIMUSim()

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
