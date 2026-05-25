// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.drivedistanceoffboard.subsystems;

import org.wpilib.command2.Command;
import org.wpilib.command2.SubsystemBase;
import org.wpilib.drive.DifferentialDrive;
import org.wpilib.examples.drivedistanceoffboard.Constants.DriveConstants;
import org.wpilib.examples.drivedistanceoffboard.ExampleSmartMotorController;
import org.wpilib.math.controller.SimpleMotorFeedforward;
import org.wpilib.math.trajectory.TrapezoidProfile;
import org.wpilib.math.trajectory.TrapezoidProfile.State;
import org.wpilib.system.RobotController;
import org.wpilib.system.Timer;
import org.wpilib.util.sendable.SendableRegistry;

public class DriveSubsystem extends SubsystemBase {
  // The motors on the left side of the drive.
  private final ExampleSmartMotorController leftLeader =
      new ExampleSmartMotorController(DriveConstants.kLeftMotor1Port);

  private final ExampleSmartMotorController leftFollower =
      new ExampleSmartMotorController(DriveConstants.kLeftMotor2Port);

  // The motors on the right side of the drive.
  private final ExampleSmartMotorController rightLeader =
      new ExampleSmartMotorController(DriveConstants.kRightMotor1Port);

  private final ExampleSmartMotorController rightFollower =
      new ExampleSmartMotorController(DriveConstants.kRightMotor2Port);

  // The feedforward controller.
  private final SimpleMotorFeedforward feedforward =
      new SimpleMotorFeedforward(DriveConstants.ks, DriveConstants.kv, DriveConstants.ka);

  // The robot's drive
  private final DifferentialDrive drive =
      new DifferentialDrive(leftLeader::setThrottle, rightLeader::setThrottle);

  // The trapezoid profile
  private final TrapezoidProfile profile =
      new TrapezoidProfile(
          new TrapezoidProfile.Constraints(
              DriveConstants.kMaxVelocity, DriveConstants.kMaxAcceleration));

  // The timer
  private final Timer timer = new Timer();

  /** Creates a new DriveSubsystem. */
  public DriveSubsystem() {
    SendableRegistry.addChild(drive, leftLeader);
    SendableRegistry.addChild(drive, rightLeader);

    // We need to invert one side of the drivetrain so that positive voltages
    // result in both sides moving forward. Depending on how your robot's
    // gearbox is constructed, you might have to invert the left side instead.
    rightLeader.setInverted(true);

    leftFollower.follow(leftLeader);
    rightFollower.follow(rightLeader);

    leftLeader.setPID(DriveConstants.kp, 0, 0);
    rightLeader.setPID(DriveConstants.kp, 0, 0);
  }

  /**
   * Drives the robot using arcade controls.
   *
   * @param fwd the commanded forward movement
   * @param rot the commanded rotation
   */
  public void arcadeDrive(double fwd, double rot) {
    drive.arcadeDrive(fwd, rot);
  }

  /**
   * Attempts to follow the given drive states using offboard PID.
   *
   * @param currentLeft The current left wheel state.
   * @param currentRight The current right wheel state.
   * @param nextLeft The next left wheel state.
   * @param nextRight The next right wheel state.
   */
  public void setDriveStates(
      TrapezoidProfile.State currentLeft,
      TrapezoidProfile.State currentRight,
      TrapezoidProfile.State nextLeft,
      TrapezoidProfile.State nextRight) {
    // Feedforward is divided by battery voltage to normalize it to [-1, 1]
    leftLeader.setSetpoint(
        ExampleSmartMotorController.PIDMode.kPosition,
        currentLeft.position,
        feedforward.calculate(currentLeft.velocity, nextLeft.velocity)
            / RobotController.getBatteryVoltage());
    rightLeader.setSetpoint(
        ExampleSmartMotorController.PIDMode.kPosition,
        currentRight.position,
        feedforward.calculate(currentLeft.velocity, nextLeft.velocity)
            / RobotController.getBatteryVoltage());
  }

  /**
   * Returns the left encoder distance.
   *
   * @return the left encoder distance
   */
  public double getLeftEncoderDistance() {
    return leftLeader.getEncoderDistance();
  }

  /**
   * Returns the right encoder distance.
   *
   * @return the right encoder distance
   */
  public double getRightEncoderDistance() {
    return rightLeader.getEncoderDistance();
  }

  /** Resets the drive encoders. */
  public void resetEncoders() {
    leftLeader.resetEncoder();
    rightLeader.resetEncoder();
  }

  /**
   * Sets the max output of the drive. Useful for scaling the drive to drive more slowly.
   *
   * @param maxOutput the maximum output to which the drive will be constrained
   */
  public void setMaxOutput(double maxOutput) {
    drive.setMaxOutput(maxOutput);
  }

  /**
   * Creates a command to drive forward a specified distance using a motion profile.
   *
   * @param distance The distance to drive forward.
   * @return A command.
   */
  public Command profiledDriveDistance(double distance) {
    return startRun(
            () -> {
              // Restart timer so profile setpoints start at the beginning
              timer.restart();
              resetEncoders();
            },
            () -> {
              // Current state never changes, so we need to use a timer to get the setpoints we need
              // to be at
              var currentTime = timer.get();
              var currentSetpoint =
                  profile.calculate(currentTime, new State(), new State(distance, 0));
              var nextSetpoint =
                  profile.calculate(
                      currentTime + DriveConstants.kDt, new State(), new State(distance, 0));
              setDriveStates(currentSetpoint, currentSetpoint, nextSetpoint, nextSetpoint);
            })
        .until(() -> profile.isFinished(0));
  }

  private double initialLeftDistance;
  private double initialRightDistance;

  /**
   * Creates a command to drive forward a specified distance using a motion profile without
   * resetting the encoders.
   *
   * @param distance The distance to drive forward.
   * @return A command.
   */
  public Command dynamicProfiledDriveDistance(double distance) {
    return startRun(
            () -> {
              // Restart timer so profile setpoints start at the beginning
              timer.restart();
              // Store distance so we know the target distance for each encoder
              initialLeftDistance = getLeftEncoderDistance();
              initialRightDistance = getRightEncoderDistance();
            },
            () -> {
              // Current state never changes for the duration of the command, so we need to use a
              // timer to get the setpoints we need to be at
              var currentTime = timer.get();
              var currentLeftSetpoint =
                  profile.calculate(
                      currentTime,
                      new State(initialLeftDistance, 0),
                      new State(initialLeftDistance + distance, 0));
              var currentRightSetpoint =
                  profile.calculate(
                      currentTime,
                      new State(initialRightDistance, 0),
                      new State(initialRightDistance + distance, 0));
              var nextLeftSetpoint =
                  profile.calculate(
                      currentTime + DriveConstants.kDt,
                      new State(initialLeftDistance, 0),
                      new State(initialLeftDistance + distance, 0));
              var nextRightSetpoint =
                  profile.calculate(
                      currentTime + DriveConstants.kDt,
                      new State(initialRightDistance, 0),
                      new State(initialRightDistance + distance, 0));
              setDriveStates(
                  currentLeftSetpoint, currentRightSetpoint, nextLeftSetpoint, nextRightSetpoint);
            })
        .until(() -> profile.isFinished(0));
  }
}
