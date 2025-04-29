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
