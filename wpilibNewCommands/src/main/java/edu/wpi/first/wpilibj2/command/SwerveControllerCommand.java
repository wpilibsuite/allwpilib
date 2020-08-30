/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj2.command;

import java.util.function.Consumer;
import java.util.function.Supplier;

import edu.wpi.first.wpilibj.Timer;
import edu.wpi.first.wpilibj.controller.HolonomicDriveController;
import edu.wpi.first.wpilibj.controller.PIDController;
import edu.wpi.first.wpilibj.controller.ProfiledPIDController;
import edu.wpi.first.wpilibj.geometry.Pose2d;
import edu.wpi.first.wpilibj.geometry.Rotation2d;
import edu.wpi.first.wpilibj.kinematics.SwerveDriveKinematics;
import edu.wpi.first.wpilibj.kinematics.SwerveModuleState;
import edu.wpi.first.wpilibj.trajectory.Trajectory;

import static edu.wpi.first.wpilibj.util.ErrorMessages.requireNonNullParam;

/**
 * A command that uses two PID controllers ({@link PIDController}) and a
 * ProfiledPIDController ({@link ProfiledPIDController}) to follow a trajectory
 * {@link Trajectory} with a swerve drive.
 *
 * <p>This command outputs the raw desired Swerve Module States ({@link SwerveModuleState})
 * in an array. The desired wheel and module rotation velocities should be taken
 * from those and used in velocity PIDs.
 *
 * <p>The robot angle controller does not follow the angle given by
 * the trajectory but rather goes to the angle given in the final state of the trajectory.
 */

@SuppressWarnings("MemberName")
public class SwerveControllerCommand extends CommandBase {
  private final Timer m_timer = new Timer();
  private final Trajectory m_trajectory;
  private final Supplier<Pose2d> m_pose;
  private final SwerveDriveKinematics m_kinematics;
  private final HolonomicDriveController m_controller;
  private final Consumer<SwerveModuleState[]> m_outputModuleStates;
  private final Supplier<Rotation2d> m_desiredRotation;

  /**
   * Constructs a new SwerveControllerCommand that when executed will follow the provided
   * trajectory. This command will not return output voltages but rather raw module states from the
   * position controllers which need to be put into a velocity PID.
   *
   * <p>Note: The controllers will *not* set the outputVolts to zero upon completion of the path-
   * this is left to the user, since it is not appropriate for paths with nonstationary endstates.
   *
   * @param trajectory         The trajectory to follow.
   * @param pose               A function that supplies the robot pose - use one of
   *                           the odometry classes to provide this.
   * @param kinematics         The kinematics for the robot drivetrain.
   * @param xController        The Trajectory Tracker PID controller
   *                           for the robot's x position.
   * @param yController        The Trajectory Tracker PID controller
   *                           for the robot's y position.
   * @param thetaController    The Trajectory Tracker PID controller
   *                           for angle for the robot.
   * @param desiredRotation    The angle that the drivetrain should be facing. This
   *                           is sampled at each time step.
   * @param outputModuleStates The raw output module states from the
   *                           position controllers.
   * @param requirements       The subsystems to require.
   */
  @SuppressWarnings("ParameterName")
  public SwerveControllerCommand(Trajectory trajectory,
                                 Supplier<Pose2d> pose,
                                 SwerveDriveKinematics kinematics,
                                 PIDController xController,
                                 PIDController yController,
                                 ProfiledPIDController thetaController,
                                 Supplier<Rotation2d> desiredRotation,
                                 Consumer<SwerveModuleState[]> outputModuleStates,
                                 Subsystem... requirements) {
    m_trajectory = requireNonNullParam(trajectory, "trajectory", "SwerveControllerCommand");
    m_pose = requireNonNullParam(pose, "pose", "SwerveControllerCommand");
    m_kinematics = requireNonNullParam(kinematics, "kinematics", "SwerveControllerCommand");

    m_controller = new HolonomicDriveController(
        requireNonNullParam(xController,
            "xController", "SwerveControllerCommand"),
        requireNonNullParam(yController,
            "xController", "SwerveControllerCommand"),
        requireNonNullParam(thetaController,
            "thetaController", "SwerveControllerCommand")
    );

    m_outputModuleStates = requireNonNullParam(outputModuleStates,
        "frontLeftOutput", "SwerveControllerCommand");

    m_desiredRotation = requireNonNullParam(desiredRotation, "desiredRotation",
        "SwerveControllerCommand");

    addRequirements(requirements);
  }

  /**
   * Constructs a new SwerveControllerCommand that when executed will follow the provided
   * trajectory. This command will not return output voltages but rather raw module states from the
   * position controllers which need to be put into a velocity PID.
   *
   * <p>Note: The controllers will *not* set the outputVolts to zero upon completion of the path-
   * this is left to the user, since it is not appropriate for paths with nonstationary endstates.
   *
   * <p>Note 2: The final rotation of the robot will be set to the rotation of
   * the final pose in the trajectory. The robot will not follow the rotations
   * from the poses at each timestep. If alternate rotation behavior is desired,
   * the other constructor with a supplier for rotation should be used.
   *
   * @param trajectory         The trajectory to follow.
   * @param pose               A function that supplies the robot pose - use one of
   *                           the odometry classes to provide this.
   * @param kinematics         The kinematics for the robot drivetrain.
   * @param xController        The Trajectory Tracker PID controller
   *                           for the robot's x position.
   * @param yController        The Trajectory Tracker PID controller
   *                           for the robot's y position.
   * @param thetaController    The Trajectory Tracker PID controller
   *                           for angle for the robot.
   * @param outputModuleStates The raw output module states from the
   *                           position controllers.
   * @param requirements       The subsystems to require.
   */
  @SuppressWarnings("ParameterName")
  public SwerveControllerCommand(Trajectory trajectory,
                                 Supplier<Pose2d> pose,
                                 SwerveDriveKinematics kinematics,
                                 PIDController xController,
                                 PIDController yController,
                                 ProfiledPIDController thetaController,
                                 Consumer<SwerveModuleState[]> outputModuleStates,
                                 Subsystem... requirements) {
    this(trajectory, pose, kinematics, xController, yController, thetaController,
        () -> trajectory.getStates().get(trajectory.getStates().size() - 1)
            .poseMeters.getRotation(),
        outputModuleStates, requirements);
  }

  @Override
  public void initialize() {
    m_timer.reset();
    m_timer.start();
  }

  @Override
  @SuppressWarnings("LocalVariableName")
  public void execute() {
    double curTime = m_timer.get();
    var desiredState = m_trajectory.sample(curTime);

    var targetChassisSpeeds = m_controller.calculate(m_pose.get(), desiredState,
        m_desiredRotation.get());
    var targetModuleStates = m_kinematics.toSwerveModuleStates(targetChassisSpeeds);

    m_outputModuleStates.accept(targetModuleStates);
  }

  @Override
  public void end(boolean interrupted) {
    m_timer.stop();
  }

  @Override
  public boolean isFinished() {
    return m_timer.hasElapsed(m_trajectory.getTotalTimeSeconds());
  }
}
