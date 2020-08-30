/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.controller;

import edu.wpi.first.wpilibj.geometry.Pose2d;
import edu.wpi.first.wpilibj.geometry.Rotation2d;
import edu.wpi.first.wpilibj.kinematics.ChassisSpeeds;
import edu.wpi.first.wpilibj.trajectory.Trajectory;

/**
 * This holonomic drive controller can be used to follow trajectories using a
 * holonomic drive train (i.e. swerve or mecanum). Holonomic trajectory following
 * is a much simpler problem to solve compared to skid-steer style drivetrains because
 * it is possible to individually control forward, sideways, and angular velocity.
 *
 * <p>The holonomic drive controller takes in one PID controller for each direction, forward
 * and sideways, and one profiled PID controller for the angular direction. Because the
 * heading dynamics are decoupled from translations, users can specify a custom heading
 * that the drivetrain should point toward. This heading reference is profiled for smoothness.
 */
@SuppressWarnings("MemberName")
public class HolonomicDriveController {
  private Pose2d m_poseError = new Pose2d();
  private Pose2d m_poseTolerance = new Pose2d();
  private boolean m_enabled = true;

  private final PIDController m_xController;
  private final PIDController m_yController;
  private final ProfiledPIDController m_thetaController;

  /**
   * Constructs a holonomic drive controller.
   *
   * @param xController     A PID Controller to respond to error in the field-relative x direction.
   * @param yController     A PID Controller to respond to error in the field-relative y direction.
   * @param thetaController A profiled PID controller to respond to error in angle.
   */
  @SuppressWarnings("ParameterName")
  public HolonomicDriveController(PIDController xController,
                                  PIDController yController,
                                  ProfiledPIDController thetaController) {
    m_xController = xController;
    m_yController = yController;
    m_thetaController = thetaController;
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
   * Sets the pose error which is considered tolerance for use with atReference().
   *
   * @param tolerance The pose error which is tolerable.
   */
  public void setTolerance(Pose2d tolerance) {
    m_poseTolerance = tolerance;
  }

  /**
   * Returns the next output of the holonomic drive controller.
   *
   * @param currentPose             The current pose.
   * @param poseRef                 The desired pose.
   * @param linearVelocityRefMeters The linear velocity reference.
   * @param angleRef                The angular reference.
   * @return The next output of the holonomic drive controller.
   */
  @SuppressWarnings("LocalVariableName")
  public ChassisSpeeds calculate(Pose2d currentPose,
                                 Pose2d poseRef,
                                 double linearVelocityRefMeters,
                                 Rotation2d angleRef) {
    // Calculate feedforward velocities (field-relative).
    double xFF = linearVelocityRefMeters * poseRef.getRotation().getCos();
    double yFF = linearVelocityRefMeters * poseRef.getRotation().getSin();
    double thetaFF = m_thetaController.calculate(currentPose.getRotation().getRadians(),
        angleRef.getRadians());

    m_poseError = poseRef.relativeTo(currentPose);

    if (!m_enabled) {
      return ChassisSpeeds.fromFieldRelativeSpeeds(xFF, yFF, thetaFF, currentPose.getRotation());
    }

    // Calculate feedback velocities (based on position error).
    double xFeedback = m_xController.calculate(currentPose.getX(), poseRef.getX());
    double yFeedback = m_yController.calculate(currentPose.getY(), poseRef.getY());

    // Return next output.
    return ChassisSpeeds.fromFieldRelativeSpeeds(xFF + xFeedback,
        yFF + yFeedback, thetaFF, currentPose.getRotation());
  }

  /**
   * Returns the next output of the holonomic drive controller.
   *
   * @param currentPose  The current pose.
   * @param desiredState The desired trajectory state.
   * @param angleRef     The desired end-angle.
   * @return The next output of the holonomic drive controller.
   */
  public ChassisSpeeds calculate(Pose2d currentPose, Trajectory.State desiredState,
                                 Rotation2d angleRef) {
    return calculate(currentPose, desiredState.poseMeters, desiredState.velocityMetersPerSecond,
        angleRef);
  }

  /**
   * Enables and disables the controller for troubleshooting problems. When calculate()
   * is called on a disabled controller, only feedforward values are returned.
   *
   * @param enabled If the controller is enabled or not.
   */
  public void setEnabled(boolean enabled) {
    m_enabled = enabled;
  }
}
