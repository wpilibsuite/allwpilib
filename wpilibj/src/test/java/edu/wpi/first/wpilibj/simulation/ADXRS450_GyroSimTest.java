// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.ADXRS450_Gyro;
import org.junit.jupiter.api.Test;

@SuppressWarnings({"TypeName"})
class ADXRS450_GyroSimTest {
  @Test
  void testCallbacks() {
    HAL.initialize(500, 0);

    try (ADXRS450_Gyro gyro = new ADXRS450_Gyro()) {
      ADXRS450_GyroSim sim = new ADXRS450_GyroSim(gyro);

      assertEquals(0, gyro.getAngle());
      assertEquals(0, gyro.getRate());

      sim.setAngle(123.456);
      sim.setRate(229.3504);

      assertEquals(123.456, gyro.getAngle());
      assertEquals(229.3504, gyro.getRate());

      gyro.reset();
      assertEquals(0, gyro.getAngle());
    }
  }
}
