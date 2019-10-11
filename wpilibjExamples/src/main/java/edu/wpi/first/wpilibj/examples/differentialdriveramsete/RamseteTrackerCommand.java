/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.differentialdriveramsete;

import edu.wpi.first.wpilibj.Timer;
import edu.wpi.first.wpilibj.controller.RamseteController;
import edu.wpi.first.wpilibj.geometry.Pose2d;
import edu.wpi.first.wpilibj.kinematics.ChassisSpeeds;
import edu.wpi.first.wpilibj.trajectory.Trajectory;
import edu.wpi.first.wpilibj2.command.CommandBase;

/**
 * Command that tracks a trajectory using the Ramsete controller.
 */
public class RamseteTrackerCommand extends CommandBase {
  private final Trajectory m_trajectory;
  private final RamseteController m_controller;
  private final Drivetrain m_drivetrain;

  // Timer to keep track of time elapsed.
  private final Timer m_timer = new Timer();

  /**
   * Constructs a RamseteTrackerCommand.
   *
   * @param trajectory The trajectory to track.
   * @param controller The controller to use.
   */
  public RamseteTrackerCommand(Trajectory trajectory, RamseteController controller,
                               Drivetrain drivetrain) {
    m_trajectory = trajectory;
    m_controller = controller;
    m_drivetrain = drivetrain;

    // Require the drivetrain subsystem.
    addRequirements(m_drivetrain);
  }

  @Override
  public void initialize() {
    // Start the timer.
    m_timer.start();
  }

  @Override
  public void execute() {
    // Get the goal point at this time.
    Trajectory.State goal = m_trajectory.sample(m_timer.get());

    // Get the robot position.
    Pose2d robotPose = m_drivetrain.getPose();

    // Update the controller
    ChassisSpeeds adjustedSpeeds = m_controller.calculate(robotPose, goal);

    // Set the speeds
    m_drivetrain.setSpeeds(adjustedSpeeds);
  }

  @Override
  public boolean isFinished() {
    // The tracking is finished when the time elapsed is greater than the
    // trajectory duration.
    return m_timer.get() > m_trajectory.getTotalTimeSeconds();
  }

  @Override
  public void end(boolean interrupted) {
    // End timer
    m_timer.stop();
  }
}
