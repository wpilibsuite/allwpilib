// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.async.subsystems;

import edu.wpi.first.math.controller.PIDController;
import edu.wpi.first.math.controller.SimpleMotorFeedforward;
import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.examples.rapidreactcommandbot.Constants.ShooterConstants;
import edu.wpi.first.wpilibj.motorcontrol.PWMSparkMax;
import edu.wpi.first.wpilibj3.command.async.AsyncCommand;
import edu.wpi.first.wpilibj3.command.async.ParallelGroup;
import edu.wpi.first.wpilibj3.command.async.Resource;

public class Shooter extends Resource {
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
        () -> {
          m_shooterMotor.disable();
          m_feederMotor.disable();
          Thread.sleep(Long.MAX_VALUE);
        })
        .named("Idle"));
  }

  /**
   * Returns a command to shoot the balls currently stored in the robot. Spins the shooter flywheel
   * up to the specified setpoint, and then runs the feeder motor.
   *
   * @param setpointRotationsPerSecond The desired shooter velocity
   */
  @SuppressWarnings("InfiniteLoopStatement")
  public AsyncCommand shootCommand(double setpointRotationsPerSecond) {
    return ParallelGroup.onDefaultScheduler().all(
      run(() -> {
        while (true) {
          AsyncCommand.yield();
          m_shooterMotor.set(
            m_shooterFeedforward.calculate(setpointRotationsPerSecond)
              + m_shooterFeedback.calculate(
              m_shooterEncoder.getRate(), setpointRotationsPerSecond));
        }
      }).named("Spin Up"),

      AsyncCommand.noHardware(() -> {
        while (true) {
          AsyncCommand.yield();
          if (m_shooterFeedback.atSetpoint()) {
            m_feederMotor.set(1);
          } else {
            m_feederMotor.set(0);
          }
        }
      }).named("Launch")
    ).named("Shoot[" + setpointRotationsPerSecond + "]");
  }
}
