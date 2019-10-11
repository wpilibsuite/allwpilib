/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.differentialdriveramsete;

import java.util.ArrayList;
import java.util.Arrays;

import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj.controller.RamseteController;
import edu.wpi.first.wpilibj.geometry.Pose2d;
import edu.wpi.first.wpilibj.geometry.Rotation2d;
import edu.wpi.first.wpilibj.trajectory.Trajectory;
import edu.wpi.first.wpilibj.trajectory.TrajectoryGenerator;
import edu.wpi.first.wpilibj.util.Units;
import edu.wpi.first.wpilibj2.command.CommandScheduler;

public class Robot extends TimedRobot {
  private final Drivetrain m_drivetrain = new Drivetrain();

  // Simple trajectory to make a short left turn.
  private final Trajectory m_trajectory = TrajectoryGenerator.generateTrajectory(
      // Waypoints
      Arrays.asList(
          new Pose2d(0, 0, new Rotation2d()),
          new Pose2d(Units.feetToMeters(5), Units.feetToMeters(5), Rotation2d.fromDegrees(90))),
      // No additional constraints
      new ArrayList<>(),
      // Start and end velocities are zero
      0.0, 0.0,
      // Max velocity is 6 feet per second.
      Units.feetToMeters(6.0),
      // Max acceleration is 6 feet per second squared.
      Units.feetToMeters(6.0),
      // The robot will not travel backward.
      false
  );

  // Ramsete Controller
  private final RamseteController m_controller = new RamseteController(2.0, 0.7);

  @Override
  public void autonomousInit() {
    // Reset the pose to the position of the first waypoint.
    m_drivetrain.reset(new Pose2d());

    // Start the command
    new RamseteTrackerCommand(m_trajectory, m_controller, m_drivetrain).schedule();
  }

  @Override
  public void autonomousPeriodic() {
    CommandScheduler.getInstance().run();
  }
}
