// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.trajectory.constraint;

import static edu.wpi.first.util.ErrorMessages.requireNonNullParam;

import edu.wpi.first.math.controller.SimpleMotorFeedforward;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.kinematics.ChassisSpeeds;
import edu.wpi.first.math.kinematics.DifferentialDriveKinematics;
import org.ejml.simple.SimpleMatrix;

/**
 * A class that enforces constraints on differential drive voltage expenditure based on the motor
 * dynamics and the drive kinematics. Ensures that the acceleration of any wheel of the robot while
 * following the trajectory is never higher than what can be achieved with the given maximum
 * voltage.
 */
public class DifferentialDriveVoltageConstraint implements TrajectoryConstraint {
  private final SimpleMotorFeedforward m_feedforward;
  private final DifferentialDriveKinematics m_kinematics;
  private final double m_maxVoltage;

  /**
   * Creates a new DifferentialDriveVoltageConstraint.
   *
   * @param feedforward A feedforward component describing the behavior of the drive.
   * @param kinematics A kinematics component describing the drive geometry.
   * @param maxVoltage The maximum voltage available to the motors while following the path. Should
   *     be somewhat less than the nominal battery voltage (12V) to account for "voltage sag" due to
   *     current draw.
   */
  public DifferentialDriveVoltageConstraint(
      SimpleMotorFeedforward feedforward,
      DifferentialDriveKinematics kinematics,
      double maxVoltage) {
    m_feedforward =
        requireNonNullParam(feedforward, "feedforward", "DifferentialDriveVoltageConstraint");
    m_kinematics =
        requireNonNullParam(kinematics, "kinematics", "DifferentialDriveVoltageConstraint");
    m_maxVoltage = maxVoltage;
  }

  @Override
  public double getMaxVelocityMetersPerSecond(
      Pose2d poseMeters, double curvatureRadPerMeter, double velocityMetersPerSecond) {
    // Create an object to represent the current chassis speeds.
    var chassisSpeeds =
        new ChassisSpeeds(
            velocityMetersPerSecond, 0, velocityMetersPerSecond * curvatureRadPerMeter);

    // Get the wheel speeds and normalize them to within the max velocity.
    var wheelSpeeds = m_kinematics.toWheelSpeeds(chassisSpeeds);
    wheelSpeeds.normalize(velocityMetersPerSecond);

    // Return the new linear chassis speed.
    return m_kinematics.toChassisSpeeds(wheelSpeeds).vxMetersPerSecond;
  }

  @Override
  @SuppressWarnings("LocalVariableName")
  public MinMax getMinMaxAccelerationMetersPerSecondSq(
      Pose2d poseMeters, double curvatureRadPerMeter, double velocityMetersPerSecond) {
    // Create an object to represent the current chassis speeds.
    var chassisSpeeds =
        new ChassisSpeeds(
            velocityMetersPerSecond, 0, velocityMetersPerSecond * curvatureRadPerMeter);

    // Get the wheel speeds and normalize them to within the max velocity.
    var wheelSpeeds = m_kinematics.toWheelSpeeds(chassisSpeeds);
    wheelSpeeds.normalize(velocityMetersPerSecond);

    // See section 15.1 of https://tavsys.net/controls-in-frc
    final double Kv = m_feedforward.kv;
    final double Ka = m_feedforward.ka;
    final var A = SimpleMatrix.diag(-Kv / Ka, -Kv / Ka);
    final var B = SimpleMatrix.diag(1.0 / Ka, 1.0 / Ka);

    var x = new SimpleMatrix(2, 1);
    x.set(0, 0, wheelSpeeds.leftMetersPerSecond);
    x.set(1, 0, wheelSpeeds.rightMetersPerSecond);

    // Get dx/dt for min u
    var u = new SimpleMatrix(2, 1);
    u.set(0, 0, -m_maxVoltage);
    u.set(1, 0, -m_maxVoltage);
    double minAccel = getAcceleration(A, B, x, u);

    // Get dx/dt for max u
    u.set(0, 0, m_maxVoltage);
    u.set(1, 0, m_maxVoltage);
    double maxAccel = getAcceleration(A, B, x, u);

    return new MinMax(minAccel, maxAccel);
  }

  /**
   * Returns the longitudinal acceleration of the differential drive chassis given its current wheel
   * state and wheel input.
   *
   * @param A System matrix.
   * @param B Input matrix.
   * @param x State vector.
   * @param u Input vector.
   */
  @SuppressWarnings({"ParameterName", "LocalVariableName"})
  private double getAcceleration(SimpleMatrix A, SimpleMatrix B, SimpleMatrix x, SimpleMatrix u) {
    final double Ks = m_feedforward.ks;
    final double Ka = m_feedforward.ka;

    var xDot = A.mult(x).plus(B.mult(u));
    xDot.set(0, 0, xDot.get(0, 0) - Ks / Ka * Math.signum(x.get(0, 0)));
    xDot.set(1, 0, xDot.get(1, 0) - Ks / Ka * Math.signum(x.get(1, 0)));
    return (xDot.get(0, 0) + xDot.get(1, 0)) / 2.0;
  }
}
