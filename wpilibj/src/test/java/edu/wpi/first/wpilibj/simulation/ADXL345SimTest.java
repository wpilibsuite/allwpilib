// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.ADXL345_I2C;
import edu.wpi.first.wpilibj.I2C;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.EnumSource;

class ADXL345SimTest {
  @ParameterizedTest
  @EnumSource(ADXL345_I2C.Range.class)
  void testInitI2C(ADXL345_I2C.Range range) {
    HAL.initialize(500, 0);

    try (ADXL345_I2C accel = new ADXL345_I2C(I2C.Port.PORT0, range)) {
      ADXL345Sim sim = new ADXL345Sim(accel);

      sim.setX(1.91);
      sim.setY(-3.405);
      sim.setZ(2.29);

      assertEquals(1.91, accel.getX());
      assertEquals(-3.405, accel.getY());
      assertEquals(2.29, accel.getZ());

      ADXL345_I2C.AllAxes allAccel = accel.getAccelerations();
      assertEquals(1.91, allAccel.XAxis);
      assertEquals(-3.405, allAccel.YAxis);
      assertEquals(2.29, allAccel.ZAxis);
    }
  }
}
