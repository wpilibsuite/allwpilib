/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.armbot.subsystems;

import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.PWMVictorSPX;
import edu.wpi.first.wpilibj.controller.ArmFeedforward;
import edu.wpi.first.wpilibj.controller.ProfiledPIDController;
import edu.wpi.first.wpilibj.trajectory.TrapezoidProfile;
import edu.wpi.first.wpilibj2.command.ProfiledPIDSubsystem;

import static edu.wpi.first.wpilibj.examples.armbot.Constants.ArmConstants.kAVoltSecondSquaredPerRad;
import static edu.wpi.first.wpilibj.examples.armbot.Constants.ArmConstants.kArmOffsetRads;
import static edu.wpi.first.wpilibj.examples.armbot.Constants.ArmConstants.kCosVolts;
import static edu.wpi.first.wpilibj.examples.armbot.Constants.ArmConstants.kEncoderDistancePerPulse;
import static edu.wpi.first.wpilibj.examples.armbot.Constants.ArmConstants.kEncoderPorts;
import static edu.wpi.first.wpilibj.examples.armbot.Constants.ArmConstants.kMaxAccelerationRadPerSecSquared;
import static edu.wpi.first.wpilibj.examples.armbot.Constants.ArmConstants.kMaxVelocityRadPerSecond;
import static edu.wpi.first.wpilibj.examples.armbot.Constants.ArmConstants.kMotorPort;
import static edu.wpi.first.wpilibj.examples.armbot.Constants.ArmConstants.kP;
import static edu.wpi.first.wpilibj.examples.armbot.Constants.ArmConstants.kSVolts;
import static edu.wpi.first.wpilibj.examples.armbot.Constants.ArmConstants.kVVoltSecondPerRad;

/**
 * A robot arm subsystem that moves with a motion profile.
 */
public class ArmSubsystem extends ProfiledPIDSubsystem {
  private final PWMVictorSPX m_motor = new PWMVictorSPX(kMotorPort);
  private final Encoder m_encoder = new Encoder(kEncoderPorts[0], kEncoderPorts[1]);
  private final ArmFeedforward m_feedforward =
      new ArmFeedforward(kSVolts, kCosVolts, kVVoltSecondPerRad, kAVoltSecondSquaredPerRad);

  /**
   * Create a new ArmSubsystem.
   */
  public ArmSubsystem() {
    super(new ProfiledPIDController(kP, 0, 0,
                                    new TrapezoidProfile.Constraints(
                                        kMaxVelocityRadPerSecond,
                                        kMaxAccelerationRadPerSecSquared)),
          0);
    m_encoder.setDistancePerPulse(kEncoderDistancePerPulse);
    // Start arm at rest in neutral position
    setGoal(kArmOffsetRads);
  }

  @Override
  public void useOutput(double output, TrapezoidProfile.State setpoint) {
    // Calculate the feedforward from the sepoint
    double feedforward = m_feedforward.calculate(setpoint.position, setpoint.velocity);
    // Add the feedforward to the PID output to get the motor output
    m_motor.setVoltage(output + feedforward);
  }

  @Override
  public double getMeasurement() {
    return m_encoder.getDistance() + kArmOffsetRads;
  }
}
