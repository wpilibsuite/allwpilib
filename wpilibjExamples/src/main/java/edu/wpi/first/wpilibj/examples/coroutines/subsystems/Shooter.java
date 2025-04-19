// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.coroutines.subsystems;

import static edu.wpi.first.units.Units.RotationsPerSecond;
import static edu.wpi.first.units.Units.Volts;

import edu.wpi.first.math.controller.PIDController;
import edu.wpi.first.math.controller.SimpleMotorFeedforward;
import edu.wpi.first.units.measure.AngularVelocity;
import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.commandsv3.RequireableResource;
import edu.wpi.first.wpilibj.examples.coroutines.Constants.ShooterConstants;
import edu.wpi.first.wpilibj.motorcontrol.PWMSparkMax;

public class Shooter extends RequireableResource {
  private final PWMSparkMax m_shooterMotor = new PWMSparkMax(ShooterConstants.kShooterMotorPort);
  private final PWMSparkMax m_feederMotor = new PWMSparkMax(ShooterConstants.kFeederMotorPort);
  private final Encoder m_shooterEncoder =
      new Encoder(
          ShooterConstants.kEncoderPorts[0],
          ShooterConstants.kEncoderPorts[1],
          ShooterConstants.kEncoderReversed);
  private final SimpleMotorFeedforward m_shooterFeedforward =
      new SimpleMotorFeedforward(
          ShooterConstants.kSVolts, ShooterConstants.kVVoltSecondsPerRotation);
  private final PIDController m_shooterFeedback = new PIDController(ShooterConstants.kP, 0.0, 0.0);

  /** The shooter subsystem for the robot. */
  public Shooter() {
    super("Shooter");
    m_shooterFeedback.setTolerance(ShooterConstants.kShooterToleranceRPS);
    m_shooterEncoder.setDistancePerPulse(ShooterConstants.kEncoderDistancePerPulse);

    // Set default command to turn off both the shooter and feeder motors, and then idle
    setDefaultCommand(
      run(
        (coroutine) -> {
          m_shooterMotor.disable();
          m_feederMotor.disable();
          coroutine.park();
        })
        .named("Idle"));
  }

  public void ramp(AngularVelocity setpoint) {
    m_shooterMotor.set(
        m_shooterFeedforward.calculate(setpoint).in(Volts)
            + m_shooterFeedback.calculate(
            m_shooterEncoder.getRate(), setpoint.in(RotationsPerSecond)));
  }

  public boolean atSetpoint() {
    return m_shooterFeedback.atSetpoint();
  }

  public void feed() {
    m_feederMotor.set(1);
  }

  public void stop() {
    m_feederMotor.set(0);
  }
}
