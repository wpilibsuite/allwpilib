// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller;

import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N2;
import edu.wpi.first.math.system.LinearSystem;

/** A calculator for generating optimal feedback gains given known control characteristics. */
public class FeedbackAnalysis {
  public record FeedbackGains(double kP, double kD) {}

  /**
   * Calculates optimal PD gains for a second-order position system.
   *
   * @param system The dynamics function for the system.
   * @param maxControlEffort Max control effort for the controller.
   * @param positionTolerance Max position tolerance for the controller.
   * @param velocityTolerance Max velocity tolerance for the controller.
   * @param controllerPeriodSeconds Period of the controller.
   * @param measurementDelaySeconds Measurement delay.
   * @return Optimal PD gains given the above system and control characteristics.
   * @see edu.wpi.first.math.system.plant.LinearSystemId
   */
  public static FeedbackGains getPositionGains(
      LinearSystem<N2, N1, N1> system,
      double maxControlEffort,
      double positionTolerance,
      double velocityTolerance,
      double controllerPeriodSeconds,
      double measurementDelaySeconds) {
    // Create an LQR with 2 states to control -- position and velocity.
    var controller =
        new LinearQuadraticRegulator<>(
            system,
            VecBuilder.fill(positionTolerance, velocityTolerance),
            VecBuilder.fill(maxControlEffort),
            controllerPeriodSeconds);
    // Compensate for any latency from sensor measurements, filtering, etc.
    controller.latencyCompensate(system, controllerPeriodSeconds, measurementDelaySeconds);

    return new FeedbackGains(controller.getK().get(0, 0), controller.getK().get(0, 1));
  }

  /**
   * Calculates an optimal P gain for a first-order velocity system.
   *
   * @param system The dynamics function for the system.
   * @param maxControlEffort Max control effort for the controller.
   * @param tolerance Max velocity tolerance for the controller.
   * @param controllerPeriodSeconds Period of the controller.
   * @param measurementDelaySeconds Measurement delay.
   * @return Optimal P gain given the above system and control characteristics.
   * @see edu.wpi.first.math.system.plant.LinearSystemId
   */
  public static double getVelocityPGain(
      LinearSystem<N1, N1, N1> system,
      double maxControlEffort,
      double tolerance,
      double controllerPeriodSeconds,
      double measurementDelaySeconds) {
    // Create an LQR controller with 1 state -- velocity.
    var controller =
        new LinearQuadraticRegulator<>(
            system,
            VecBuilder.fill(tolerance),
            VecBuilder.fill(maxControlEffort),
            controllerPeriodSeconds);
    // Compensate for any latency from sensor measurements, filtering, etc.
    controller.latencyCompensate(system, controllerPeriodSeconds, measurementDelaySeconds);

    return controller.getK().get(0, 0);
  }
}
