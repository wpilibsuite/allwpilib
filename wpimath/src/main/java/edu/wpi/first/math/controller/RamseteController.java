// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller;

import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.kinematics.ChassisSpeeds;
import edu.wpi.first.math.trajectory.Trajectory;

/**
 * Ramsete is a nonlinear time-varying feedback controller for unicycle models that drives the model
 * to a desired pose along a two-dimensional trajectory. Why would we need a nonlinear control law
 * in addition to the linear ones we have used so far like PID? If we use the original approach with
 * PID controllers for left and right position and velocity states, the controllers only deal with
 * the local pose. If the robot deviates from the path, there is no way for the controllers to
 * correct and the robot may not reach the desired global pose. This is due to multiple endpoints
 * existing for the robot which have the same encoder path arc lengths.
 *
 * <p>Instead of using wheel path arc lengths (which are in the robot's local coordinate frame),
 * nonlinear controllers like pure pursuit and Ramsete use global pose. The controller uses this
 * extra information to guide a linear reference tracker like the PID controllers back in by
 * adjusting the references of the PID controllers.
 *
 * <p>The paper "Control of Wheeled Mobile Robots: An Experimental Overview" describes a nonlinear
 * controller for a wheeled vehicle with unicycle-like kinematics; a global pose consisting of x, y,
 * and theta; and a desired pose consisting of x_d, y_d, and theta_d. We call it Ramsete because
 * that's the acronym for the title of the book it came from in Italian ("Robotica Articolata e
 * Mobile per i SErvizi e le TEcnologie").
 *
 * <p>See <a href="https://file.tavsys.net/control/controls-engineering-in-frc.pdf">Controls
 * Engineering in the FIRST Robotics Competition</a> section on Ramsete unicycle controller for a
 * derivation and analysis.
 */
public class RamseteController {
  private final double m_b;

  private final double m_zeta;

  private Pose2d m_poseError = Pose2d.kZero;
  private Pose2d m_poseTolerance = Pose2d.kZero;
  private boolean m_enabled = true;

  /**
   * Construct a Ramsete unicycle controller.
   *
   * @param b Tuning parameter (b &gt; 0 rad²/m²) for which larger values make convergence more
   *     aggressive like a proportional term.
   * @param zeta Tuning parameter (0 rad⁻¹ &lt; zeta &lt; 1 rad⁻¹) for which larger values provide
   *     more damping in response.
   * @deprecated Use LTVUnicycleController instead.
   */
  @Deprecated(since = "2025", forRemoval = true)
  public RamseteController(double b, double zeta) {
    m_b = b;
    m_zeta = zeta;
  }

  /**
   * Construct a Ramsete unicycle controller. The default arguments for b and zeta of 2.0 rad²/m²
   * and 0.7 rad⁻¹ have been well-tested to produce desirable results.
   *
   * @deprecated Use LTVUnicycleController instead.
   */
  @Deprecated(since = "2025", forRemoval = true)
  public RamseteController() {
    this(2.0, 0.7);
  }

  /**
   * Returns true if the pose error is within tolerance of the reference.
   *
   * @return True if the pose error is within tolerance of the reference.
   */
  public boolean atReference() {
    final var eTranslate = m_poseError.getTranslation();
    final var eRotate = m_poseError.getRotation();
    final var tolTranslate = m_poseTolerance.getTranslation();
    final var tolRotate = m_poseTolerance.getRotation();
    return Math.abs(eTranslate.getX()) < tolTranslate.getX()
        && Math.abs(eTranslate.getY()) < tolTranslate.getY()
        && Math.abs(eRotate.getRadians()) < tolRotate.getRadians();
  }

  /**
   * Sets the pose error which is considered tolerable for use with atReference().
   *
   * @param poseTolerance Pose error which is tolerable.
   */
  public void setTolerance(Pose2d poseTolerance) {
    m_poseTolerance = poseTolerance;
  }

  /**
   * Returns the next output of the Ramsete controller.
   *
   * <p>The reference pose, linear velocity, and angular velocity should come from a drivetrain
   * trajectory.
   *
   * @param currentPose The current pose.
   * @param poseRef The desired pose.
   * @param linearVelocityRefMeters The desired linear velocity in meters per second.
   * @param angularVelocityRefRadiansPerSecond The desired angular velocity in radians per second.
   * @return The next controller output.
   */
  public ChassisSpeeds calculate(
      Pose2d currentPose,
      Pose2d poseRef,
      double linearVelocityRefMeters,
      double angularVelocityRefRadiansPerSecond) {
    if (!m_enabled) {
      return new ChassisSpeeds(linearVelocityRefMeters, 0.0, angularVelocityRefRadiansPerSecond);
    }

    m_poseError = poseRef.relativeTo(currentPose);

    // Aliases for equation readability
    final double eX = m_poseError.getX();
    final double eY = m_poseError.getY();
    final double eTheta = m_poseError.getRotation().getRadians();
    final double vRef = linearVelocityRefMeters;
    final double omegaRef = angularVelocityRefRadiansPerSecond;

    // k = 2ζ√(ω_ref² + b v_ref²)
    double k = 2.0 * m_zeta * Math.sqrt(Math.pow(omegaRef, 2) + m_b * Math.pow(vRef, 2));

    // v_cmd = v_ref cos(e_θ) + k e_x
    // ω_cmd = ω_ref + k e_θ + b v_ref sinc(e_θ) e_y
    return new ChassisSpeeds(
        vRef * m_poseError.getRotation().getCos() + k * eX,
        0.0,
        omegaRef + k * eTheta + m_b * vRef * sinc(eTheta) * eY);
  }

  /**
   * Returns the next output of the Ramsete controller.
   *
   * <p>The reference pose, linear velocity, and angular velocity should come from a drivetrain
   * trajectory.
   *
   * @param currentPose The current pose.
   * @param desiredState The desired pose, linear velocity, and angular velocity from a trajectory.
   * @return The next controller output.
   */
  public ChassisSpeeds calculate(Pose2d currentPose, Trajectory.State desiredState) {
    return calculate(
        currentPose,
        desiredState.poseMeters,
        desiredState.velocityMetersPerSecond,
        desiredState.velocityMetersPerSecond * desiredState.curvatureRadPerMeter);
  }

  /**
   * Enables and disables the controller for troubleshooting purposes.
   *
   * @param enabled If the controller is enabled or not.
   */
  public void setEnabled(boolean enabled) {
    m_enabled = enabled;
  }

  /**
   * Returns sin(x) / x.
   *
   * @param x Value of which to take sinc(x).
   */
  private static double sinc(double x) {
    if (Math.abs(x) < 1e-9) {
      return 1.0 - 1.0 / 6.0 * x * x;
    } else {
      return Math.sin(x) / x;
    }
  }
}
