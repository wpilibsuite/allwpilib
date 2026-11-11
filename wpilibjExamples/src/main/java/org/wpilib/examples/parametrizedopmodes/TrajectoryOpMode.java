// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.parametrizedopmodes;

import org.wpilib.math.kinematics.DifferentialDriveWheelVelocities;
import org.wpilib.math.trajectory.DifferentialTrajectory;
import org.wpilib.opmode.PeriodicOpMode;
import org.wpilib.system.Timer;

/**
 * An autonomous opmode that follows a trajectory. It has no {@code @Autonomous} annotation because
 * the annotation scan can only construct opmodes with no arguments or a robot argument; {@link
 * Robot} registers instances of this class with different trajectories instead.
 */
public class TrajectoryOpMode extends PeriodicOpMode {
  private final Robot robot;
  private final DifferentialTrajectory trajectory;
  private final Timer timer = new Timer();

  /**
   * Creates a trajectory opmode.
   *
   * @param robot The robot whose drivetrain follows the trajectory.
   * @param trajectory The trajectory to follow.
   */
  public TrajectoryOpMode(Robot robot, DifferentialTrajectory trajectory) {
    this.robot = robot;
    this.trajectory = trajectory;
  }

  /** Called once when the robot transitions from disabled to enabled. */
  @Override
  public void start() {
    timer.restart();
  }

  /** Called periodically while this opmode is enabled. */
  @Override
  public void periodic() {
    var sample = trajectory.sampleAt(timer.get());
    robot.drivetrain.setVelocities(
        new DifferentialDriveWheelVelocities(sample.leftVelocity, sample.rightVelocity));
  }
}
