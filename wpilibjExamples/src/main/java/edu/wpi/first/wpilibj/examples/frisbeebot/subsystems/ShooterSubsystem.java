/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.frisbeebot.subsystems;

import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.PWMVictorSPX;
import edu.wpi.first.wpilibj.controller.PIDController;
import edu.wpi.first.wpilibj.controller.SimpleMotorFeedforward;
import edu.wpi.first.wpilibj2.command.PIDSubsystem;

import static edu.wpi.first.wpilibj.examples.frisbeebot.Constants.ShooterConstants.kD;
import static edu.wpi.first.wpilibj.examples.frisbeebot.Constants.ShooterConstants.kEncoderDistancePerPulse;
import static edu.wpi.first.wpilibj.examples.frisbeebot.Constants.ShooterConstants.kEncoderPorts;
import static edu.wpi.first.wpilibj.examples.frisbeebot.Constants.ShooterConstants.kEncoderReversed;
import static edu.wpi.first.wpilibj.examples.frisbeebot.Constants.ShooterConstants.kFeederMotorPort;
import static edu.wpi.first.wpilibj.examples.frisbeebot.Constants.ShooterConstants.kFeederSpeed;
import static edu.wpi.first.wpilibj.examples.frisbeebot.Constants.ShooterConstants.kI;
import static edu.wpi.first.wpilibj.examples.frisbeebot.Constants.ShooterConstants.kP;
import static edu.wpi.first.wpilibj.examples.frisbeebot.Constants.ShooterConstants.kSVolts;
import static edu.wpi.first.wpilibj.examples.frisbeebot.Constants.ShooterConstants.kShooterMotorPort;
import static edu.wpi.first.wpilibj.examples.frisbeebot.Constants.ShooterConstants.kShooterTargetRPS;
import static edu.wpi.first.wpilibj.examples.frisbeebot.Constants.ShooterConstants.kShooterToleranceRPS;
import static edu.wpi.first.wpilibj.examples.frisbeebot.Constants.ShooterConstants.kVVoltSecondsPerRotation;

public class ShooterSubsystem extends PIDSubsystem {
  private final PWMVictorSPX m_shooterMotor = new PWMVictorSPX(kShooterMotorPort);
  private final PWMVictorSPX m_feederMotor = new PWMVictorSPX(kFeederMotorPort);
  private final Encoder m_shooterEncoder =
      new Encoder(kEncoderPorts[0], kEncoderPorts[1], kEncoderReversed);
  private final SimpleMotorFeedforward m_shooterFeedforward
      = new SimpleMotorFeedforward(kSVolts, kVVoltSecondsPerRotation);

  /**
   * The shooter subsystem for the robot.
   */
  public ShooterSubsystem() {
    super(new PIDController(kP, kI, kD));
    getController().setTolerance(kShooterToleranceRPS);
    m_shooterEncoder.setDistancePerPulse(kEncoderDistancePerPulse);
    setSetpoint(kShooterTargetRPS);
  }

  @Override
  public void useOutput(double output, double setpoint) {
    m_shooterMotor.setVoltage(output + m_shooterFeedforward.calculate(setpoint));
  }

  @Override
  public double getMeasurement() {
    return m_shooterEncoder.getRate();
  }

  public boolean atSetpoint() {
    return m_controller.atSetpoint();
  }

  public void runFeeder() {
    m_feederMotor.set(kFeederSpeed);
  }

  public void stopFeeder() {
    m_feederMotor.set(0);
  }
}
