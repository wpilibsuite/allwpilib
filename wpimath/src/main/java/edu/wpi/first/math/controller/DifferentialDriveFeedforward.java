// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller;

import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.numbers.N2;
import edu.wpi.first.math.system.LinearSystem;
import edu.wpi.first.math.system.plant.LinearSystemId;

public class DifferentialDriveFeedforward {

  private final LinearSystem<N2, N2, N2> m_plant;

  public DifferentialDriveFeedforward(
      double kVLinear, double kALinear, double kVAngular, double kAAngular, double trackwidth) {
    m_plant =
        LinearSystemId.identifyDrivetrainSystem(
            kVLinear, kALinear, kVAngular, kAAngular, trackwidth);
  }

  public DifferentialDriveFeedforward(
      double kVLinear, double kALinear, double kVAngular, double kAAngular) {
    m_plant = LinearSystemId.identifyDrivetrainSystem(kVLinear, kALinear, kVAngular, kAAngular);
  }

  // need to add constructor which creates default trackwidth

  public DifferentialDriveWheelVoltages calculate(
      double currentLVelocity,
      double nextLVelocity,
      double currentRVelocity,
      double nextRVelocity,
      double dtSeconds) {

    var feedforward = new LinearPlantInversionFeedforward<>(m_plant, dtSeconds);
    var r = VecBuilder.fill(currentLVelocity, currentRVelocity);
    var nextR = VecBuilder.fill(nextLVelocity, nextRVelocity);
    var u = feedforward.calculate(r, nextR);
    return new DifferentialDriveWheelVoltages(u.get(0, 0), u.get(1, 0));
  }
}
