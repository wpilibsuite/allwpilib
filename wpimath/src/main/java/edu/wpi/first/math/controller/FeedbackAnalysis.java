package edu.wpi.first.math.controller;

import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.system.LinearSystem;

/**
 * A calculator for generating optimal PD gains given known control characteristics.
 *
 * <p>Implementation is taken from SysID.
 */
public class FeedbackAnalysis {
  public record FeedbackGains(double kP, double kD) {}

  public static FeedbackGains getPositionGains(
      double kV,
      double kA,
      double maxControlEffort,
      double positionTolerance,
      double velocityTolerance,
      double controllerPeriodSeconds,
      double measurementDelaySeconds) {
    if (!Double.isFinite(kV) || !Double.isFinite(kA)) {
      throw new IllegalArgumentException("kV and kA must be finite numbers!");
    }
    if (kV < 0) {
      throw new IllegalArgumentException("kV must be greater than or equal to zero!");
    }
    if (kA < 0) {
      throw new IllegalArgumentException("kA must be greater than or equal to zero!");
    }

    // If acceleration requires no effort, velocity becomes an input for position
    // control. We choose an appropriate model in this case to avoid numerical
    // instabilities in the LQR.
    if (kA >= 1e-7) {
      // Create a position system from our feedforward gains.
      var system =
          new LinearSystem<>(
              new Matrix<>(Nat.N2(), Nat.N2(), new double[] {0.0, 1.0, 0.0, -kV / kA}),
              new Matrix<>(Nat.N2(), Nat.N1(), new double[] {0.0, 1.0 / kA}),
              new Matrix<>(Nat.N1(), Nat.N2(), new double[] {1.0, 0.0}),
              new Matrix<>(Nat.N1(), Nat.N1(), new double[] {0.0}));
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

    // This is our special model to avoid instabilities in the LQR.
    var system =
        new LinearSystem<>(
            new Matrix<>(Nat.N1(), Nat.N1(), new double[] {0.0}),
            new Matrix<>(Nat.N1(), Nat.N1(), new double[] {1.0}),
            new Matrix<>(Nat.N1(), Nat.N1(), new double[] {1.0}),
            new Matrix<>(Nat.N1(), Nat.N1(), new double[] {0.0}));
    // Create an LQR with one state -- position.
    var controller =
        new LinearQuadraticRegulator<>(
            system,
            VecBuilder.fill(positionTolerance),
            VecBuilder.fill(maxControlEffort),
            controllerPeriodSeconds);
    // Compensate for any latency from sensor measurements, filtering, etc.
    controller.latencyCompensate(system, controllerPeriodSeconds, measurementDelaySeconds);

    return new FeedbackGains(kV * controller.getK().get(0, 0), 0);
  }

  public static double getVelocityPGain(
      double kV,
      double kA,
      double maxControlEffort,
      double tolerance,
      double controllerPeriodSeconds,
      double measurementDelaySeconds) {
    if (!Double.isFinite(kV) || !Double.isFinite(kA)) {
      throw new IllegalArgumentException("kV and kA must be finite numbers!");
    }
    if (kV < 0) {
      throw new IllegalArgumentException("kV must be greater than or equal to 0!");
    }
    if (kA < 0) {
      throw new IllegalArgumentException("kA must be greater than or equal to 0!");
    }

    // If acceleration for velocity control requires no effort, the feedback
    // control gains approach zero. We special-case it here because numerical
    // instabilities arise in LQR otherwise.
    if (kA <= 1e-7) {
      return 0;
    }

    // Create a velocity system from our feedforward gains.
    var system =
        new LinearSystem<>(
            new Matrix<>(Nat.N1(), Nat.N1(), new double[] {-kV / kA}),
            new Matrix<>(Nat.N1(), Nat.N1(), new double[] {1.0 / kA}),
            new Matrix<>(Nat.N1(), Nat.N1(), new double[] {1.0}),
            new Matrix<>(Nat.N1(), Nat.N1(), new double[] {0.0}));
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
