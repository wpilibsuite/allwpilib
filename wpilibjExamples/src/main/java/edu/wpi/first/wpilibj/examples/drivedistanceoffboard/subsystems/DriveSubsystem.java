// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.drivedistanceoffboard.subsystems;

import edu.wpi.first.math.controller.SimpleMotorFeedforward;
import edu.wpi.first.math.trajectory.TrapezoidProfile;
import edu.wpi.first.math.trajectory.TrapezoidProfile.State;
import edu.wpi.first.util.sendable.SendableRegistry;
import edu.wpi.first.wpilibj.Timer;
import edu.wpi.first.wpilibj.drive.DifferentialDrive;
import edu.wpi.first.wpilibj.examples.drivedistanceoffboard.Constants.DriveConstants;
import edu.wpi.first.wpilibj.examples.drivedistanceoffboard.ExampleSmartMotorController;
import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.SubsystemBase;

public class DriveSubsystem extends SubsystemBase {
  // The motors on the left side of the drive.
  private final ExampleSmartMotorController m_leftLeader =
      new ExampleSmartMotorController(DriveConstants.kLeftMotor1Port);

  private final ExampleSmartMotorController m_leftFollower =
      new ExampleSmartMotorController(DriveConstants.kLeftMotor2Port);

  // The motors on the right side of the drive.
  private final ExampleSmartMotorController m_rightLeader =
      new ExampleSmartMotorController(DriveConstants.kRightMotor1Port);

  private final ExampleSmartMotorController m_rightFollower =
      new ExampleSmartMotorController(DriveConstants.kRightMotor2Port);

  // The feedforward controller.
  private final SimpleMotorFeedforward m_feedforward =
      new SimpleMotorFeedforward(
          DriveConstants.ksVolts,
          DriveConstants.kvVoltSecondsPerMeter,
          DriveConstants.kaVoltSecondsSquaredPerMeter);

  // The robot's drive
  private final DifferentialDrive m_drive =
      new DifferentialDrive(m_leftLeader::set, m_rightLeader::set);

  /** Creates a new DriveSubsystem. */
  public DriveSubsystem() {
    SendableRegistry.addChild(m_drive, m_leftLeader);
    SendableRegistry.addChild(m_drive, m_rightLeader);

    // We need to invert one side of the drivetrain so that positive voltages
    // result in both sides moving forward. Depending on how your robot's
    // gearbox is constructed, you might have to invert the left side instead.
    m_rightLeader.setInverted(true);

    m_leftFollower.follow(m_leftLeader);
    m_rightFollower.follow(m_rightLeader);

    m_leftLeader.setPID(DriveConstants.kp, 0, 0);
    m_rightLeader.setPID(DriveConstants.kp, 0, 0);
  }

  /**
   * Drives the robot using arcade controls.
   *
   * @param fwd the commanded forward movement
   * @param rot the commanded rotation
   */
  public void arcadeDrive(double fwd, double rot) {
    m_drive.arcadeDrive(fwd, rot);
  }

  /**
   * Attempts to follow the given drive states using offboard PID.
   *
   * @param left The left wheel state.
   * @param right The right wheel state.
   */
  public void setDriveStates(TrapezoidProfile.State left, TrapezoidProfile.State right) {
    m_leftLeader.setSetpoint(
        ExampleSmartMotorController.PIDMode.kPosition,
        left.position,
        m_feedforward.calculate(left.velocity));
    m_rightLeader.setSetpoint(
        ExampleSmartMotorController.PIDMode.kPosition,
        right.position,
        m_feedforward.calculate(right.velocity));
  }

  /**
   * Returns the left encoder distance.
   *
   * @return the left encoder distance
   */
  public double getLeftEncoderDistance() {
    return m_leftLeader.getEncoderDistance();
  }

  /**
   * Returns the right encoder distance.
   *
   * @return the right encoder distance
   */
  public double getRightEncoderDistance() {
    return m_rightLeader.getEncoderDistance();
  }

  /** Resets the drive encoders. */
  public void resetEncoders() {
    m_leftLeader.resetEncoder();
    m_rightLeader.resetEncoder();
  }

  /**
   * Sets the max output of the drive. Useful for scaling the drive to drive more slowly.
   *
   * @param maxOutput the maximum output to which the drive will be constrained
   */
  public void setMaxOutput(double maxOutput) {
    m_drive.setMaxOutput(maxOutput);
  }

  /**
   * Creates a command to drive forward a specified distance using a motion profile.
   *
   * @param distance The distance to drive forward.
   * @return A command.
   */
  public Command profiledDriveDistance(double distance) {
    var profile =
        new TrapezoidProfile(
            new TrapezoidProfile.Constraints(
                DriveConstants.kMaxSpeedMetersPerSecond,
                DriveConstants.kMaxAccelerationMetersPerSecondSquared));
    var timer = new Timer();
    return startRun(
            () -> {
              // Restart timer so profile setpoints start at the beginning
              timer.restart();
              resetEncoders();
            },
            () -> {
              // Current state never changes, so we need to use a timer to get the setpoints we need
              // to be at
              var setpoint = profile.calculate(timer.get(), new State(), new State(3, 0));
              setDriveStates(setpoint, setpoint);
            })
        .until(() -> profile.isFinished(0));
  }

  private double m_initialLeftDistance;
  private double m_initialRightDistance;

  /**
   * Creates a command to drive forward a specified distance using a motion profile without
   * resetting the encoders.
   *
   * @param distance The distance to drive forward.
   * @return A command.
   */
  public Command dynamicProfiledDriveDistance(double distance) {
    var profile =
        new TrapezoidProfile(
            new TrapezoidProfile.Constraints(
                DriveConstants.kMaxSpeedMetersPerSecond,
                DriveConstants.kMaxAccelerationMetersPerSecondSquared));
    return startRun(
            () -> {
              // Store distance so we know the target distance for each encoder
              m_initialLeftDistance = getLeftEncoderDistance();
              m_initialRightDistance = getRightEncoderDistance();
            },
            () -> {
              // Current state matches the actual state of the bot, so the profile needs to look 20
              // milliseconds ahead for the next setpoint
              var leftSetpoint =
                  profile.calculate(
                      DriveConstants.kDt,
                      new State(getLeftEncoderDistance(), m_leftLeader.getEncoderRate()),
                      new State(m_initialLeftDistance + 3, 0));
              var rightSetpoint =
                  profile.calculate(
                      DriveConstants.kDt,
                      new State(getRightEncoderDistance(), m_rightLeader.getEncoderRate()),
                      new State(m_initialRightDistance + 3, 0));
              setDriveStates(leftSetpoint, rightSetpoint);
            })
        .until(() -> profile.isFinished(0));
  }
}
