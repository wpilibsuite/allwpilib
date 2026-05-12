from wpilib import OnboardIMU
from wpilib.simulation import OnboardIMUSim

def test_sim_device() -> None:

    imu = OnboardIMU(OnboardIMU.MountOrientation.FLAT)
    sim = OnboardIMUSim()

    assert 0.0 == imu.getAngleX()
    assert 0.0 == imu.getAngleY()
    assert 0.0 == imu.getAngleZ()
    assert 0.0 == imu.getGyroRateX()
    assert 0.0 == imu.getGyroRateY()
    assert 0.0 == imu.getGyroRateZ()
    assert 0.0 == imu.getAccelX()
    assert 0.0 == imu.getAccelY()
    assert 0.0 == imu.getAccelZ()
    
    sim.setAngleX(1)
    sim.setAngleY(2)
    sim.setAngleZ(3)
    
    sim.setGyroRateX(3.504)
    sim.setGyroRateY(1.91)
    sim.setGyroRateZ(22.9)
    
    sim.setAccelX(-1)
    sim.setAccelY(-2)
    sim.setAccelZ(-3)

    assert 1.0 == imu.getAngleX()
    assert 2.0 == imu.getAngleY()
    assert 3.0 == imu.getAngleZ()

    assert 3.504 == imu.getGyroRateX()
    assert 1.91 == imu.getGyroRateY()
    assert 22.9 == imu.getGyroRateZ()

    assert -1.0 == imu.getAccelX()
    assert -2.0 == imu.getAccelY()
    assert -3.0 == imu.getAccelZ()