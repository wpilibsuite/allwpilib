/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.controller;

import edu.wpi.first.wpilibj.geometry.Pose2d;
import edu.wpi.first.wpilibj.kinematics.ChassisSpeeds;
import edu.wpi.first.wpilibj.trajectory.Trajectory;

/**
 * Ramsete is a nonlinear time-varying feedback controller for unicycle models
 * that drives the model to a desired pose along a two-dimensional trajectory.
 * Why would we need a nonlinear control law in addition to the linear ones we
 * have used so far like PID? If we use the original approach with PID
 * controllers for left and right position and velocity states, the controllers
 * only deal with the local pose. If the robot deviates from the path, there is
 * no way for the controllers to correct and the robot may not reach the desired
 * global pose. This is due to multiple endpoints existing for the robot which
 * have the same encoder path arc lengths.
 *
 * <p>Instead of using wheel path arc lengths (which are in the robot's local
 * coordinate frame), nonlinear controllers like pure pursuit and Ramsete use
 * global pose. The controller uses this extra information to guide a linear
 * reference tracker like the PID controllers back in by adjusting the
 * references of the PID controllers.
 *
 * <p>The paper "Control of Wheeled Mobile Robots: An Experimental Overview"
 * describes a nonlinear controller for a wheeled vehicle with unicycle-like
 * kinematics; a global pose consisting of x, y, and theta; and a desired pose
 * consisting of x_d, y_d, and theta_d. We call it Ramsete because that's the
 * acronym for the title of the book it came from in Italian ("Robotica
 * Articolata e Mobile per i SErvizi e le TEcnologie").
 *
 * <p>See
 * <a href="https://file.tavsys.net/control/controls-engineering-in-frc.pdf">
 * Controls Engineering in the FIRST Robotics Competition</a> section on Ramsete
 * unicycle controller for a derivation and analysis.
 */
public class RamseteController {
  @SuppressWarnings("MemberName")
  private final double m_b;
  @SuppressWarnings("MemberName")
  private final double m_zeta;

  private Pose2d m_poseError = new Pose2d();
  private Pose2d m_poseTolerance = new Pose2d();

  /**
   * Construct a Ramsete unicycle controller.
   *
   * @param b    Tuning parameter (b &gt; 0) for which larger values make convergence more
   *             aggressive like a proportional term.
   * @param zeta Tuning parameter (0 &lt; zeta &lt; 1) for which larger values provide more damping
   *             in response.
   */
  @SuppressWarnings("ParameterName")
  public RamseteController(double b, double zeta) {
    m_b = b;
    m_zeta = zeta;
  }

  /**
   * Returns true if the pose error is within tolerance of the reference.
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
   * Sets the pose error which is considered tolerable for use with
   * atReference().
   *
   * @param poseTolerance Pose error which is tolerable.
   */
  public void setTolerance(Pose2d poseTolerance) {
    m_poseTolerance = poseTolerance;
  }

  /**
   * Returns the next output of the Ramsete controller.
   *
   * <p>The reference pose, linear velocity, and angular velocity should come
   * from a drivetrain trajectory.
   *
   * @param currentPose                        The current pose.
   * @param poseRef                            The desired pose.
   * @param linearVelocityRefMeters            The desired linear velocity in meters.
   * @param angularVelocityRefRadiansPerSecond The desired angular velocity in meters.
   */
  @SuppressWarnings("LocalVariableName")
  public ChassisSpeeds calculate(Pose2d currentPose,
                                 Pose2d poseRef,
                                 double linearVelocityRefMeters,
                                 double angularVelocityRefRadiansPerSecond) {
    m_poseError = poseRef.relativeTo(currentPose);

    // Aliases for equation readability
    final double eX = m_poseError.getTranslation().getX();
    final double eY = m_poseError.getTranslation().getY();
    final double eTheta = m_poseError.getRotation().getRadians();
    final double vRef = linearVelocityRefMeters;
    final double omegaRef = angularVelocityRefRadiansPerSecond;

    double k = 2.0 * m_zeta * Math.sqrt(Math.pow(omegaRef, 2) + m_b * Math.pow(vRef, 2));

    return new ChassisSpeeds(vRef * m_poseError.getRotation().getCos() + k * eX,
                             0.0,
                             omegaRef + k * eTheta + m_b * vRef * sinc(eTheta) * eY);
  }

  /**
   * Returns the next output of the Ramsete controller.
   *
   * <p>The reference pose, linear velocity, and angular velocity should come
   * from a drivetrain trajectory.
   *
   * @param currentPose  The current pose.
   * @param desiredState The desired pose, linear velocity, and angular velocity
   *                     from a trajectory.
   */
  @SuppressWarnings("LocalVariableName")
  public ChassisSpeeds calculate(Pose2d currentPose, Trajectory.State desiredState) {
    return calculate(currentPose, desiredState.poseMeters, desiredState.velocityMetersPerSecond,
        desiredState.velocityMetersPerSecond * desiredState.curvatureRadPerMeter);
  }

  /**
   * Returns sin(x) / x.
   *
   * @param x Value of which to take sinc(x).
   */
  @SuppressWarnings("ParameterName")
  private static double sinc(double x) {
    if (Math.abs(x) < 1e-9) {
      return 1.0 - 1.0 / 6.0 * x * x;
    } else {
      return Math.sin(x) / x;
    }
  }
}
