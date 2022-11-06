// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N2;
import edu.wpi.first.math.system.LinearSystem;
import edu.wpi.first.math.system.plant.LinearSystemId;
import org.junit.jupiter.api.Test;

class DifferentialDriveFeedforwardTest {
  private static final double kVLinear = 1.0;
  private static final double kALinear = 1.0;
  private static final double kVAngular = 1.0;
  private static final double kAAngular = 1.0;
  private static final double trackwidth = 1.0;
  private static final double dtSeconds = 0.02;

  @Test
  void testCalculate() {
    DifferentialDriveFeedforward differentialDriveFeedforward =
        new DifferentialDriveFeedforward(kVLinear, kALinear, kVAngular, kAAngular, trackwidth);
    LinearSystem<N2, N2, N2> plant =
        LinearSystemId.identifyDrivetrainSystem(
            kVLinear, kALinear, kVAngular, kAAngular, trackwidth);
    for (int cLVelocity = -4; cLVelocity <= 4; cLVelocity += 2) {
      for (int cRVelocity = -4; cRVelocity <= 4; cRVelocity += 2) {
        for (int nLVelocity = -4; nLVelocity <= 4; nLVelocity += 2) {
          for (int nRVelocity = -4; nRVelocity <= 4; nRVelocity += 2) {
            DifferentialDriveWheelVoltages u =
                differentialDriveFeedforward.calculate(
                    cLVelocity, nLVelocity, cRVelocity, nRVelocity, dtSeconds);
            Matrix<N2, N1> y =
                plant.calculateX(
                    VecBuilder.fill(cLVelocity, cRVelocity),
                    VecBuilder.fill(u.left, u.right),
                    dtSeconds);
            // left drivetrain check
            assertEquals(y.get(0, 0), nLVelocity, 1e-6);
            // right drivetrain check)
            assertEquals(y.get(1, 0), nRVelocity, 1e-6);
          }
        }
      }
    }
  }
}
