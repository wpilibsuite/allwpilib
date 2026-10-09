// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.imu;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.wpilib.hardware.hal.HAL;
import org.wpilib.math.geometry.Quaternion;
import org.wpilib.math.geometry.Rotation3d;
import org.wpilib.simulation.OnboardIMUSim;

class OnboardIMUTest {
  @BeforeEach
  @AfterEach
  void resetData() {
    HAL.initialize();
    OnboardIMUSim.setAngleX(0);
    OnboardIMUSim.setAngleY(0);
    OnboardIMUSim.setAngleZ(0);
    OnboardIMUSim.setGyroRateX(0);
    OnboardIMUSim.setGyroRateY(0);
    OnboardIMUSim.setGyroRateZ(0);
    OnboardIMUSim.setAccelX(0);
    OnboardIMUSim.setAccelY(0);
    OnboardIMUSim.setAccelZ(0);
    OnboardIMUSim.setYaw(0);
  }

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
    assertEquals(new Quaternion(), imu.getQuaternion());

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
    var rotation = new Rotation3d(1, 2, 3);
    assertEquals(rotation, imu.getRotation3d());
    imu.resetYaw();
    assertEquals(0, imu.getYawRadians());
    OnboardIMUSim.setYaw(2);
    assertEquals(0.766, imu.getRotation2d().getRadians(), 1e-9);
    assertEquals(3, imu.getAngleZ());
    assertEquals(rotation, imu.getRotation3d());
  }
}
