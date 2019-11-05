/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj2.command;

import static edu.wpi.first.wpilibj.util.ErrorMessages.requireNonNullParam;

import java.util.function.Consumer;
import java.util.function.Supplier;

import edu.wpi.first.wpilibj.Timer;
import edu.wpi.first.wpilibj.controller.PIDController;
import edu.wpi.first.wpilibj.controller.ProfiledPIDController;
import edu.wpi.first.wpilibj.geometry.Pose2d;
import edu.wpi.first.wpilibj.kinematics.ChassisSpeeds;
import edu.wpi.first.wpilibj.kinematics.SwerveDriveKinematics;
import edu.wpi.first.wpilibj.kinematics.SwerveModuleState;
import edu.wpi.first.wpilibj.trajectory.Trajectory;

/**
 * A command that uses two PID controllers ({@link PIDController}) and a ProfiledPIDController ({@link ProfiledPIDController}) to follow a trajectory
 * {@link Trajectory} with a swerve drive.
 *
 * <p>The command handles trajectory-following, Velocity PID calculations, and feedforwards internally. This
 * is intended to be a more-or-less "complete solution" that can be used by teams without a great
 * deal of controls expertise.
 *
 * <p>Advanced teams seeking more flexibility (for example, those who wish to use the onboard
 * PID functionality of a "smart" motor controller) may use the secondary constructor that omits
 * the PID and feedforward functionality, returning only the raw module states from the position PID controllers.
 *
 * <p>The robot angle controller does not follow the angle given by
 * the trajectory but rather goes to the angle given in the final state of the trajectory.
 */

public class SwerveFollowerCommand extends CommandBase {
  private final Timer m_timer = new Timer();
  private Pose2d m_finalPose;

  private final Trajectory m_trajectory;
  private final Supplier<Pose2d> m_pose;
  private final SwerveDriveKinematics m_kinematics;
  private final PIDController m_xController;
  private final PIDController m_yController;
  private final ProfiledPIDController m_thetaController;
  private final Consumer<SwerveModuleState[]> m_outputModuleStates;

  /**
   * Constructs a new SwerveFollowerCommand that, when executed, will follow the provided trajectory.
   * This command will not return output voltages but rather raw module states from the position controllers which need to be put into a velocty PID.
   *
   * <p>Note: The controllers will *not* set the outputVolts to zero upon completion of the path-
   * this
   * is left to the user, since it is not appropriate for paths with nonstationary endstates.
   *
   * <p>Note2: The rotation controller will calculate the rotation based on the final pose in the trajectory, not the poses at each time step.
   *
   * @param trajectory                        The trajectory to follow.
   * @param pose                              A function that supplies the robot pose - use one of
   *                                          the odometry classes to provide this.
   * @param kinematics                        The kinematics for the robot drivetrain.
   * @param xController                       The Trajectory Tracker PID controller for the robot's x position.
   * @param yController                       The Trajectory Tracker PID controller for the robot's y position.
   * @param thetaController                   The Trajectory Tracker PID controller for angle for the robot.
   * @param outputModuleStates                The raw output module states from the position controllers.
   * @param requirements                      The subsystems to require.
   */

  public SwerveFollowerCommand(
                        Trajectory trajectory,
                        Supplier<Pose2d> pose,
                        SwerveDriveKinematics kinematics,
                        PIDController xController,
                        PIDController yController,
                        ProfiledPIDController thetaController,

                        Consumer<SwerveModuleState[]> outputModuleStates,
                        Subsystem... requirements) {
    m_trajectory = requireNonNullParam(trajectory, "trajectory", "SwerveFollowerCommand");
    m_pose = requireNonNullParam(pose, "pose", "SwerveFollowerCommand");
    m_kinematics = requireNonNullParam(kinematics, "kinematics", "SwerveFollowerCommand");

    m_xController = requireNonNullParam(xController, "xController", "SwerveFollowerCommand");
    m_yController = requireNonNullParam(yController, "xController", "SwerveFollowerCommand");
    m_thetaController = requireNonNullParam(thetaController, "thetaController", "SwerveFollowerCommand");

    m_outputModuleStates = requireNonNullParam(outputModuleStates, "frontLeftOutput", "SwerveFollowerCommand");
    addRequirements(requirements);
  }

  @Override
  public void initialize() {
    m_finalPose = m_trajectory.sample(m_trajectory.getTotalTimeSeconds()).poseMeters;

    m_timer.reset();
    m_timer.start();
  }

  @Override
  public void execute() {
    double curTime = m_timer.get();

    var m_desiredState = m_trajectory.sample(curTime);
    var m_desiredPose = m_desiredState.poseMeters;

    var m_poseError = m_desiredPose.relativeTo(m_pose.get());

    double targetXVel = m_xController.calculate(
      m_pose.get().getTranslation().getX(),
       m_desiredPose.getTranslation().getX());

    double targetYVel = m_yController.calculate(
      m_pose.get().getTranslation().getY(),
       m_desiredPose.getTranslation().getY());

    double targetAngularVel = m_thetaController.calculate(
      m_pose.get().getRotation().getRadians(),
       m_finalPose.getRotation().getRadians());
       // The robot will go to the desired rotation of the final pose in the trajectory,
       // not following the poses at individual states.

    double vRef = m_desiredState.velocityMetersPerSecond;

    targetXVel += vRef * Math.sin(m_poseError.getRotation().getRadians());
    targetYVel += vRef * Math.cos(m_poseError.getRotation().getRadians());

    var targetChassisSpeeds = new ChassisSpeeds(targetXVel, targetYVel, targetAngularVel);

    var targetModuleStates = m_kinematics.toSwerveModuleStates(targetChassisSpeeds);

    m_outputModuleStates.accept(targetModuleStates);

  }

  @Override
  public void end(boolean interrupted) {
    m_timer.stop();
  }

  @Override
  public boolean isFinished() {
    return m_timer.hasPeriodPassed(m_trajectory.getTotalTimeSeconds());
  }
}