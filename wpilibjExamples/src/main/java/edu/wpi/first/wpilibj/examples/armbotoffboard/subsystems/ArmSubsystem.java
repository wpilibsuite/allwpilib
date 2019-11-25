/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.armbotoffboard.subsystems;

import edu.wpi.first.wpilibj.controller.ArmFeedforward;
import edu.wpi.first.wpilibj.trajectory.TrapezoidProfile;
import edu.wpi.first.wpilibj2.command.TrapezoidProfileSubsystem;

import edu.wpi.first.wpilibj.examples.armbotoffboard.ExampleSmartMotorController;

import static edu.wpi.first.wpilibj.examples.armbotoffboard.Constants.ArmConstants.kAVoltSecondSquaredPerRad;
import static edu.wpi.first.wpilibj.examples.armbotoffboard.Constants.ArmConstants.kArmOffsetRads;
import static edu.wpi.first.wpilibj.examples.armbotoffboard.Constants.ArmConstants.kCosVolts;
import static edu.wpi.first.wpilibj.examples.armbotoffboard.Constants.ArmConstants.kMaxAccelerationRadPerSecSquared;
import static edu.wpi.first.wpilibj.examples.armbotoffboard.Constants.ArmConstants.kMaxVelocityRadPerSecond;
import static edu.wpi.first.wpilibj.examples.armbotoffboard.Constants.ArmConstants.kMotorPort;
import static edu.wpi.first.wpilibj.examples.armbotoffboard.Constants.ArmConstants.kP;
import static edu.wpi.first.wpilibj.examples.armbotoffboard.Constants.ArmConstants.kSVolts;
import static edu.wpi.first.wpilibj.examples.armbotoffboard.Constants.ArmConstants.kVVoltSecondPerRad;

/**
 * A robot arm subsystem that moves with a motion profile.
 */
public class ArmSubsystem extends TrapezoidProfileSubsystem {
  private final ExampleSmartMotorController m_motor = new ExampleSmartMotorController(kMotorPort);
  private final ArmFeedforward m_feedforward =
      new ArmFeedforward(kSVolts, kCosVolts, kVVoltSecondPerRad, kAVoltSecondSquaredPerRad);

  /**
   * Create a new ArmSubsystem.
   */
  public ArmSubsystem() {
    super(new TrapezoidProfile.Constraints(kMaxVelocityRadPerSecond,
                                           kMaxAccelerationRadPerSecSquared),
          kArmOffsetRads);
    m_motor.setPID(kP, 0, 0);
  }

  @Override
  public void useState(TrapezoidProfile.State setpoint) {
    // Calculate the feedforward from the sepoint
    double feedforward = m_feedforward.calculate(setpoint.position, setpoint.velocity);
    // Add the feedforward to the PID output to get the motor output
    m_motor.setSetpoint(ExampleSmartMotorController.PIDMode.kPosition,
                        setpoint.position,
                        feedforward / 12.0);
  }
}
