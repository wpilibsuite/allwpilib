// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.imu;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;
import org.wpilib.simulation.OnboardIMUSim;

class OnboardIMUTest {
  @Test
  void testOnboardIMU() {
    OnboardIMU imu = new OnboardIMU(OnboardIMU.MountOrientation.FLAT);

    assertEquals(0.0, imu.getAngleX());
    assertEquals(0.0, imu.getAngleY());
    assertEquals(0.0, imu.getAngleZ());

    assertEquals(0.0, imu.getGyroRateX());
    assertEquals(0.0, imu.getGyroRateY());
    assertEquals(0.0, imu.getGyroRateZ());

    assertEquals(0.0, imu.getAccelX());
    assertEquals(0.0, imu.getAccelY());
    assertEquals(0.0, imu.getAccelZ());

    assertEquals(0.0, imu.getYawRadians());

    OnboardIMUSim.setAngleX(1);
    OnboardIMUSim.setAngleY(2);
    OnboardIMUSim.setAngleZ(3);

    OnboardIMUSim.setGyroRateX(3.504);
    OnboardIMUSim.setGyroRateY(1.91);
    OnboardIMUSim.setGyroRateZ(22.9);

    OnboardIMUSim.setAccelX(-1);
    OnboardIMUSim.setAccelY(-2);
    OnboardIMUSim.setAccelZ(-3);

    OnboardIMUSim.setYaw(1.234);

    assertEquals(1.0, imu.getAngleX());
    assertEquals(2.0, imu.getAngleY());
    assertEquals(3.0, imu.getAngleZ());

    assertEquals(3.504, imu.getGyroRateX());
    assertEquals(1.91, imu.getGyroRateY());
    assertEquals(22.9, imu.getGyroRateZ());

    assertEquals(-1.0, imu.getAccelX());
    assertEquals(-2.0, imu.getAccelY());
    assertEquals(-3.0, imu.getAccelZ());

    assertEquals(1.234, imu.getYawRadians());
  }
}
