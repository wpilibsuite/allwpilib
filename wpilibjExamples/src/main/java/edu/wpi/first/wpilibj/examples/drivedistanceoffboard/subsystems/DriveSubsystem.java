/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.drivedistanceoffboard.subsystems;

import edu.wpi.first.wpilibj.controller.SimpleMotorFeedforward;
import edu.wpi.first.wpilibj.drive.DifferentialDrive;
import edu.wpi.first.wpilibj.trajectory.TrapezoidProfile;
import edu.wpi.first.wpilibj2.command.SubsystemBase;

import edu.wpi.first.wpilibj.examples.drivedistanceoffboard.ExampleSmartMotorController;

import static edu.wpi.first.wpilibj.examples.drivedistanceoffboard.Constants.DriveConstants.kLeftMotor1Port;
import static edu.wpi.first.wpilibj.examples.drivedistanceoffboard.Constants.DriveConstants.kLeftMotor2Port;
import static edu.wpi.first.wpilibj.examples.drivedistanceoffboard.Constants.DriveConstants.kRightMotor1Port;
import static edu.wpi.first.wpilibj.examples.drivedistanceoffboard.Constants.DriveConstants.kRightMotor2Port;
import static edu.wpi.first.wpilibj.examples.drivedistanceoffboard.Constants.DriveConstants.kaVoltSecondsSquaredPerMeter;
import static edu.wpi.first.wpilibj.examples.drivedistanceoffboard.Constants.DriveConstants.kp;
import static edu.wpi.first.wpilibj.examples.drivedistanceoffboard.Constants.DriveConstants.ksVolts;
import static edu.wpi.first.wpilibj.examples.drivedistanceoffboard.Constants.DriveConstants.kvVoltSecondsPerMeter;

public class DriveSubsystem extends SubsystemBase {
  // The motors on the left side of the drive.
  private final ExampleSmartMotorController m_leftMaster =
      new ExampleSmartMotorController(kLeftMotor1Port);

  private final ExampleSmartMotorController m_leftSlave =
      new ExampleSmartMotorController(kLeftMotor2Port);

  private final ExampleSmartMotorController m_rightMaster =
      new ExampleSmartMotorController(kRightMotor1Port);

  private final ExampleSmartMotorController m_rightSlave =
      new ExampleSmartMotorController(kRightMotor2Port);

  private final SimpleMotorFeedforward m_feedforward =
      new SimpleMotorFeedforward(ksVolts,
                                 kvVoltSecondsPerMeter,
                                 kaVoltSecondsSquaredPerMeter);

  // The robot's drive
  private final DifferentialDrive m_drive = new DifferentialDrive(m_leftMaster, m_rightMaster);

  /**
   * Creates a new DriveSubsystem.
   */
  public DriveSubsystem() {
    m_leftSlave.follow(m_leftMaster);
    m_rightSlave.follow(m_rightMaster);

    m_leftMaster.setPID(kp, 0, 0);
    m_rightMaster.setPID(kp, 0, 0);
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
    m_leftMaster.setSetpoint(ExampleSmartMotorController.PIDMode.kPosition,
                             left.position,
                             m_feedforward.calculate(left.velocity));
    m_rightMaster.setSetpoint(ExampleSmartMotorController.PIDMode.kPosition,
                              right.position,
                              m_feedforward.calculate(right.velocity));
  }

  /**
   * Returns the left encoder distance.
   *
   * @return the left encoder distance
   */
  public double getLeftEncoderDistance() {
    return m_leftMaster.getEncoderDistance();
  }

  /**
   * Returns the right encoder distance.
   *
   * @return the right encoder distance
   */
  public double getRightEncoderDistance() {
    return m_rightMaster.getEncoderDistance();
  }

  /**
   * Resets the drive encoders.
   */
  public void resetEncoders() {
    m_leftMaster.resetEncoder();
    m_rightMaster.resetEncoder();
  }

  /**
   * Sets the max output of the drive.  Useful for scaling the drive to drive more slowly.
   *
   * @param maxOutput the maximum output to which the drive will be constrained
   */
  public void setMaxOutput(double maxOutput) {
    m_drive.setMaxOutput(maxOutput);
  }
}
