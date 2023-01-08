// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.ADXL362;
import edu.wpi.first.wpilibj.SPI;
import edu.wpi.first.wpilibj.interfaces.Accelerometer;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.EnumSource;

class ADXL362SimTest {
  @ParameterizedTest
  @EnumSource(Accelerometer.Range.class)
  void testAccel(Accelerometer.Range range) {
    HAL.initialize(500, 0);

    try (ADXL362 accel = new ADXL362(SPI.Port.kMXP, range)) {
      assertEquals(0, accel.getX());
      assertEquals(0, accel.getY());
      assertEquals(0, accel.getZ());

      ADXL362Sim sim = new ADXL362Sim(accel);
      sim.setX(1.91);
      sim.setY(-3.405);
      sim.setZ(2.29);

      assertEquals(1.91, accel.getX());
      assertEquals(-3.405, accel.getY());
      assertEquals(2.29, accel.getZ());

      ADXL362.AllAxes allAccel = accel.getAccelerations();
      assertEquals(1.91, allAccel.XAxis);
      assertEquals(-3.405, allAccel.YAxis);
      assertEquals(2.29, allAccel.ZAxis);
    }
  }
}
