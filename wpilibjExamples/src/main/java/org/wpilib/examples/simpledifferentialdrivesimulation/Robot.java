// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.simpledifferentialdrivesimulation;

import java.util.List;
import org.wpilib.driverstation.Gamepad;
import org.wpilib.framework.TimedRobot;
import org.wpilib.math.controller.LTVUnicycleController;
import org.wpilib.math.filter.SlewRateLimiter;
import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.kinematics.ChassisVelocities;
import org.wpilib.math.trajectory.Trajectory;
import org.wpilib.math.trajectory.TrajectoryConfig;
import org.wpilib.math.trajectory.TrajectoryGenerator;
import org.wpilib.system.Timer;

public class Robot extends TimedRobot {
  private final Gamepad controller = new Gamepad(0);

  // Slew rate limiters to make joystick inputs more gentle; 1/3 sec from 0
  // to 1.
  private final SlewRateLimiter velocityLimiter = new SlewRateLimiter(3);
  private final SlewRateLimiter rotLimiter = new SlewRateLimiter(3);

  private final Drivetrain drive = new Drivetrain();
  private final LTVUnicycleController feedback = new LTVUnicycleController(0.020);
  private final Timer timer = new Timer();
  private final Trajectory trajectory;

  /** Called once at the beginning of the robot program. */
  public Robot() {
    trajectory =
        TrajectoryGenerator.generateTrajectory(
            new Pose2d(2, 2, Rotation2d.kZero),
            List.of(),
            new Pose2d(6, 4, Rotation2d.kZero),
            new TrajectoryConfig(2, 2));
  }

  @Override
  public void robotPeriodic() {
    drive.periodic();
  }

  @Override
  public void autonomousInit() {
    timer.restart();
    drive.resetOdometry(trajectory.getInitialPose());
  }

  @Override
  public void autonomousPeriodic() {
    double elapsed = timer.get();
    Trajectory.State reference = trajectory.sample(elapsed);
    ChassisVelocities velocities = feedback.calculate(drive.getPose(), reference);
    drive.drive(velocities.vx, velocities.omega);
  }

  @Override
  public void teleopPeriodic() {
    // Get the x velocity. We are inverting this because gamepads return
    // negative values when we push forward.
    double xVelocity = -velocityLimiter.calculate(controller.getLeftY()) * Drivetrain.kMaxVelocity;

    // Get the rate of angular rotation. We are inverting this because we want a
    // positive value when we pull to the left (remember, CCW is positive in
    // mathematics). Xbox controllers return positive values when you pull to
    // the right by default.
    double rot = -rotLimiter.calculate(controller.getRightX()) * Drivetrain.kMaxAngularVelocity;
    drive.drive(xVelocity, rot);
  }

  @Override
  public void simulationPeriodic() {
    drive.simulationPeriodic();
  }
}
