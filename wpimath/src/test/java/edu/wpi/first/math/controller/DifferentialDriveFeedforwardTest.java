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
  void testCalculateWithTrackwidth() {
    DifferentialDriveFeedforward differentialDriveFeedforward =
        new DifferentialDriveFeedforward(kVLinear, kALinear, kVAngular, kAAngular, trackwidth);
    LinearSystem<N2, N2, N2> plant =
        LinearSystemId.identifyDrivetrainSystem(
            kVLinear, kALinear, kVAngular, kAAngular, trackwidth);
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
                    dtSeconds);
            Matrix<N2, N1> nextX =
                plant.calculateX(
                    VecBuilder.fill(currentLeftVelocity, currentRightVelocity),
                    VecBuilder.fill(u.left, u.right),
                    dtSeconds);
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
        new DifferentialDriveFeedforward(kVLinear, kALinear, kVAngular, kAAngular);
    LinearSystem<N2, N2, N2> plant =
        LinearSystemId.identifyDrivetrainSystem(kVLinear, kALinear, kVAngular, kAAngular);
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
                    dtSeconds);
            Matrix<N2, N1> nextX =
                plant.calculateX(
                    VecBuilder.fill(currentLeftVelocity, currentRightVelocity),
                    VecBuilder.fill(u.left, u.right),
                    dtSeconds);
            assertEquals(nextX.get(0, 0), nextLeftVelocity, 1e-6);
            assertEquals(nextX.get(1, 0), nextRightVelocity, 1e-6);
          }
        }
      }
    }
  }
}
