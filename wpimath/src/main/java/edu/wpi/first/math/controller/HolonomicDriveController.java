// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller;

import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Translation2d;
import edu.wpi.first.math.kinematics.ChassisSpeeds;
import edu.wpi.first.math.trajectory.Trajectory;
import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableBuilder;
import edu.wpi.first.util.sendable.SendableRegistry;

/**
 * This holonomic drive controller can be used to follow trajectories using a holonomic drivetrain
 * (i.e. swerve or mecanum). Holonomic trajectory following is a much simpler problem to solve
 * compared to skid-steer style drivetrains because it is possible to individually control forward,
 * sideways, and angular velocity.
 *
 * <p>The holonomic drive controller takes in one PID controller for each direction, forward and
 * sideways, and one profiled PID controller for the angular direction. Because the heading dynamics
 * are decoupled from translations, users can specify a custom heading that the drivetrain should
 * point toward. This heading reference is profiled for smoothness.
 */
@SuppressWarnings("MemberName")
public class HolonomicDriveController implements Sendable {
  private Pose2d m_poseError = new Pose2d();
  private Rotation2d m_rotationError = new Rotation2d();
  private Pose2d m_poseTolerance = new Pose2d();
  private Pose2d m_measurement = new Pose2d();
  private Pose2d m_poseReference = new Pose2d();
  private Rotation2d m_headingReference = new Rotation2d();

  private boolean m_enabled = true;

  private final PIDController m_xController;
  private final PIDController m_yController;
  private final ProfiledPIDController m_thetaController;

  private boolean m_firstRun = true;

  /**
   * Constructs a holonomic drive controller.
   *
   * @param xController A PID Controller to respond to error in the field-relative x direction.
   * @param yController A PID Controller to respond to error in the field-relative y direction.
   * @param thetaController A profiled PID controller to respond to error in angle.
   */
  @SuppressWarnings("ParameterName")
  public HolonomicDriveController(
      PIDController xController, PIDController yController, ProfiledPIDController thetaController) {
    m_xController = xController;
    m_yController = yController;
    m_thetaController = thetaController;

    SendableRegistry.addChild(this, m_xController, m_yController, m_thetaController);
  }

  /**
   * Returns true if the pose error is within tolerance of the reference.
   *
   * @return True if the pose error is within tolerance of the reference.
   */
  public boolean atReference() {
    final var eTranslate = m_poseError.getTranslation();
    final var eRotate = m_rotationError;
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
   * Gets the pose error which is considered tolerable by atReference().
   *
   * @return Pose error which is tolerable.
   */
  public Pose2d getTolerance() {
    return m_poseTolerance;
  }

  /**
   * Gets the current pose error.
   *
   * @return Most recent pose error.
   */
  public Pose2d getError() {
    return m_poseError;
  }

  /**
   * Gets the current pose measurement.
   *
   * @return Most recent pose measurement.
   */
  public Pose2d getMeasurement() {
    return m_measurement;
  }

  /**
   * Returns the next output of the holonomic drive controller.
   *
   * @param currentPose The current pose.
   * @param poseRef The desired pose.
   * @param linearVelocityRefMeters The desired heading (the heading of poseRef is ignored).
   * @param headingRef The desired heading.
   * @return The next output of the holonomic drive controller.
   */
  @SuppressWarnings("LocalVariableName")
  public ChassisSpeeds calculate(
      Pose2d currentPose, Pose2d poseRef, double linearVelocityRefMeters, Rotation2d headingRef) {
    m_measurement = currentPose;
    m_poseReference = poseRef;
    m_headingReference = headingRef;
    // If this is the first run, then we need to reset the theta controller to the current pose's
    // heading.
    if (m_firstRun) {
      m_thetaController.reset(currentPose.getRotation().getRadians());
      m_firstRun = false;
    }

    // Calculate feedforward velocities (field-relative).
    double xFF = linearVelocityRefMeters * poseRef.getRotation().getCos();
    double yFF = linearVelocityRefMeters * poseRef.getRotation().getSin();
    double thetaFF =
        m_thetaController.calculate(
            currentPose.getRotation().getRadians(), headingRef.getRadians());

    m_poseError = poseRef.relativeTo(currentPose);
    m_rotationError = headingRef.minus(currentPose.getRotation());

    if (!m_enabled) {
      return ChassisSpeeds.fromFieldRelativeSpeeds(xFF, yFF, thetaFF, currentPose.getRotation());
    }

    // Calculate feedback velocities (based on position error).
    double xFeedback = m_xController.calculate(currentPose.getX(), poseRef.getX());
    double yFeedback = m_yController.calculate(currentPose.getY(), poseRef.getY());

    // Return next output.
    return ChassisSpeeds.fromFieldRelativeSpeeds(
        xFF + xFeedback, yFF + yFeedback, thetaFF, currentPose.getRotation());
  }

  /**
   * Returns the next output of the holonomic drive controller.
   *
   * @param currentPose The current pose.
   * @param desiredState The desired trajectory state.
   * @param angleRef The desired end-angle.
   * @return The next output of the holonomic drive controller.
   */
  public ChassisSpeeds calculate(
      Pose2d currentPose, Trajectory.State desiredState, Rotation2d angleRef) {
    return calculate(
        currentPose, desiredState.poseMeters, desiredState.velocityMetersPerSecond, angleRef);
  }

  /**
   * Enables and disables the controller for troubleshooting problems. When calculate() is called on
   * a disabled controller, only feedforward values are returned.
   *
   * @param enabled If the controller is enabled or not.
   */
  public void setEnabled(boolean enabled) {
    m_enabled = enabled;
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("HolonomicDriveController");
    builder.addDoubleProperty(
        "toleranceX",
        () -> m_poseTolerance.getX(),
        x ->
            setTolerance(
                new Pose2d(
                    new Translation2d(x, m_poseTolerance.getY()), m_poseTolerance.getRotation())));
    builder.addDoubleProperty(
        "toleranceY",
        () -> m_poseTolerance.getY(),
        y ->
            setTolerance(
                new Pose2d(
                    new Translation2d(m_poseTolerance.getX(), y), m_poseTolerance.getRotation())));
    builder.addDoubleProperty(
        "toleranceDegrees",
        () -> m_poseTolerance.getRotation().getDegrees(),
        rotation ->
            setTolerance(
                new Pose2d(m_poseTolerance.getTranslation(), Rotation2d.fromDegrees(rotation))));
    builder.addDoubleProperty("referenceX", () -> m_poseReference.getX(), null);
    builder.addDoubleProperty("referenceY", () -> m_poseReference.getY(), null);
    builder.addDoubleProperty("referenceDegrees", () -> m_headingReference.getDegrees(), null);
    builder.addDoubleProperty("measurementX", () -> m_measurement.getX(), null);
    builder.addDoubleProperty("measurementY", () -> m_measurement.getY(), null);
    builder.addDoubleProperty(
        "measurementDegrees", () -> m_measurement.getRotation().getDegrees(), null);
    builder.addDoubleProperty("errorX", () -> m_poseError.getX(), null);
    builder.addDoubleProperty("errorY", () -> m_poseError.getY(), null);
    builder.addDoubleProperty("errorDegrees", () -> m_poseError.getRotation().getDegrees(), null);
    builder.addBooleanProperty("atReference", this::atReference, null);
  }
}
