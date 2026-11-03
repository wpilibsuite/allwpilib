// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.controller;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;
import org.wpilib.math.linalg.Matrix;
import org.wpilib.math.linalg.VecBuilder;
import org.wpilib.math.numbers.N1;
import org.wpilib.math.numbers.N2;
import org.wpilib.math.system.LinearSystem;
import org.wpilib.math.system.Models;

class DifferentialDriveFeedforwardTest {
  private static final double kvLinear = 1.0;
  private static final double kaLinear = 1.0;
  private static final double kvAngular = 1.0;
  private static final double kaAngular = 1.0;
  private static final double trackwidth = 1.0;
  private static final double dt = 0.02;

  @Test
  void testCalculateWithTrackwidth() {
    DifferentialDriveFeedforward differentialDriveFeedforward =
        new DifferentialDriveFeedforward(kvLinear, kaLinear, kvAngular, kaAngular, trackwidth);
    LinearSystem<N2, N2, N2> plant =
        Models.differentialDriveFromSysId(kvLinear, kaLinear, kvAngular, kaAngular, trackwidth);
    for (int currentLeftVelocity = -4; currentLeftVelocity <= 4; currentLeftVelocity += 2) {
      for (int currentRightVelocity = -4; currentRightVelocity <= 4; currentRightVelocity += 2) {
        for (int nextLeftVelocity = -4; nextLeftVelocity <= 4; nextLeftVelocity += 2) {
          for (int nextRightVelocity = -4; nextRightVelocity <= 4; nextRightVelocity += 2) {
            DifferentialDriveWheelVoltages u =
                differentialDriveFeedforward.calculate(
                    currentLeftVelocity,
                    nextLeftVelocity,
                    currentRightVelocity,
                    nextRightVelocity,
                    dt);
            Matrix<N2, N1> nextX =
                plant.calculateX(
                    VecBuilder.fill(currentLeftVelocity, currentRightVelocity),
                    VecBuilder.fill(u.left, u.right),
                    dt);
            assertEquals(nextX.get(0, 0), nextLeftVelocity, 1e-6);
            assertEquals(nextX.get(1, 0), nextRightVelocity, 1e-6);
          }
        }
      }
    }
  }

  @Test
  void testCalculateWithoutTrackwidth() {
    DifferentialDriveFeedforward differentialDriveFeedforward =
        new DifferentialDriveFeedforward(kvLinear, kaLinear, kvAngular, kaAngular);
    LinearSystem<N2, N2, N2> plant =
        Models.differentialDriveFromSysId(kvLinear, kaLinear, kvAngular, kaAngular);
    for (int currentLeftVelocity = -4; currentLeftVelocity <= 4; currentLeftVelocity += 2) {
      for (int currentRightVelocity = -4; currentRightVelocity <= 4; currentRightVelocity += 2) {
        for (int nextLeftVelocity = -4; nextLeftVelocity <= 4; nextLeftVelocity += 2) {
          for (int nextRightVelocity = -4; nextRightVelocity <= 4; nextRightVelocity += 2) {
            DifferentialDriveWheelVoltages u =
                differentialDriveFeedforward.calculate(
                    currentLeftVelocity,
                    nextLeftVelocity,
                    currentRightVelocity,
                    nextRightVelocity,
                    dt);
            Matrix<N2, N1> nextX =
                plant.calculateX(
                    VecBuilder.fill(currentLeftVelocity, currentRightVelocity),
                    VecBuilder.fill(u.left, u.right),
                    dt);
            assertEquals(nextX.get(0, 0), nextLeftVelocity, 1e-6);
            assertEquals(nextX.get(1, 0), nextRightVelocity, 1e-6);
          }
        }
      }
    }
  }
}
