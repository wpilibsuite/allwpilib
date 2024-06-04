// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.Rotation3d;
import edu.wpi.first.math.util.Units;
import edu.wpi.first.wpilibj.ADIS16470_IMU;
import edu.wpi.first.wpilibj.ADIS16470_IMU.IMUAxis;
import org.junit.jupiter.api.Test;

@SuppressWarnings({"TypeName"})
class ADIS16470_IMUSimTest {
  @Test
  void testCallbacks() {
    try (ADIS16470_IMU gyro = new ADIS16470_IMU()) {
      ADIS16470_IMUSim sim = new ADIS16470_IMUSim(gyro);

      assertEquals(0, gyro.getAngle());
      assertEquals(0, gyro.getRate());

      sim.setGyroOrientation(new Rotation3d(0, 0, Units.degreesToRadians(123.456)));
      sim.setGyroRateZ(229.3504);

      assertEquals(123.456, gyro.getAngle(), 0.0001);
      assertEquals(229.3504, gyro.getRate());

      Rotation3d orientation = new Rotation3d(1, 2, 3);
      sim.setGyroOrientation(orientation);
      assertEquals(gyro.getRotation3d(), orientation);
    }
  }

  @Test
  void testOffset() {
    try (ADIS16470_IMU gyro = new ADIS16470_IMU()) {
      Rotation3d offsetZ = new Rotation3d(0, 0, Units.degreesToRadians(90));
      gyro.reset(offsetZ);
      assertEquals(offsetZ, gyro.getRotation3d());
      assertEquals(90, gyro.getAngle(), 0.0001);

      Rotation3d offsetYZ =
          new Rotation3d(0, Units.degreesToRadians(90), Units.degreesToRadians(90));
      gyro.reset(offsetYZ);
      assertEquals(90, gyro.getAngle(IMUAxis.kY), 0.0001);
      assertEquals(offsetYZ, gyro.getRotation3d());
    }
  }
}
