// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller;

import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.numbers.N2;
import edu.wpi.first.math.system.LinearSystem;
import edu.wpi.first.math.system.plant.LinearSystemId;

/** A helper class which computes the feedforward outputs for a differential drive drivetrain. */
public class DifferentialDriveFeedforward {
  private final LinearSystem<N2, N2, N2> m_plant;

  /**
   * Creates a new DifferentialDriveFeedforward with the specified parameters.
   *
   * @param kVLinear The linear velocity gain in volts per (meters per second).
   * @param kALinear The linear acceleration gain in volts per (meters per second squared).
   * @param kVAngular The angular velocity gain in volts per (radians per second).
   * @param kAAngular The angular acceleration gain in volts per (radians per second squared).
   * @param trackwidth The distance between the differential drive's left and right wheels, in
   *     meters.
   */
  public DifferentialDriveFeedforward(
      double kVLinear, double kALinear, double kVAngular, double kAAngular, double trackwidth) {
    m_plant =
        LinearSystemId.identifyDrivetrainSystem(
            kVLinear, kALinear, kVAngular, kAAngular, trackwidth);
  }

  /**
   * Creates a new DifferentialDriveFeedforward with the specified parameters.
   *
   * @param kVLinear The linear velocity gain in volts per (meters per second).
   * @param kALinear The linear acceleration gain in volts per (meters per second squared).
   * @param kVAngular The angular velocity gain in volts per (radians per second).
   * @param kAAngular The angular acceleration gain in volts per (radians per second squared).
   */
  public DifferentialDriveFeedforward(
      double kVLinear, double kALinear, double kVAngular, double kAAngular) {
    m_plant = LinearSystemId.identifyDrivetrainSystem(kVLinear, kALinear, kVAngular, kAAngular);
  }

  /**
   * Calculates the differential drive feedforward inputs given velocity setpoints.
   *
   * @param currentLVelocity The current left velocity of the differential drive in meters/second.
   * @param nextLVelocity The next left velocity of the differential drive in meters/second.
   * @param currentRVelocity The current right velocity of the differential drive in meters/second.
   * @param nextRVelocity The next right velocity of the differential drive in meters/second.
   * @param dtSeconds Discretization timestep.
   * @return A DifferentialDriveWheelVoltages object containing the computed feedforward voltages.
   */
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
