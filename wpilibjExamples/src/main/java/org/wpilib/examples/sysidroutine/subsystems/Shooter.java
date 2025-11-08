// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.sysidroutine.subsystems;

import static org.wpilib.units.Units.Rotations;
import static org.wpilib.units.Units.RotationsPerSecond;
import static org.wpilib.units.Units.Volts;

import java.util.function.DoubleSupplier;
import org.wpilib.command2.Command;
import org.wpilib.command2.SubsystemBase;
import org.wpilib.command2.sysid.SysIdRoutine;
import org.wpilib.examples.sysidroutine.Constants.ShooterConstants;
import org.wpilib.hardware.motor.PWMSparkMax;
import org.wpilib.hardware.rotation.Encoder;
import org.wpilib.math.controller.PIDController;
import org.wpilib.math.controller.SimpleMotorFeedforward;
import org.wpilib.system.RobotController;

public class Shooter extends SubsystemBase {
  // The motor on the shooter wheel .
  private final PWMSparkMax m_shooterMotor = new PWMSparkMax(ShooterConstants.kShooterMotorPort);

  // The motor on the feeder wheels.
  private final PWMSparkMax m_feederMotor = new PWMSparkMax(ShooterConstants.kFeederMotorPort);

  // The shooter wheel encoder
  private final Encoder m_shooterEncoder =
      new Encoder(
          ShooterConstants.kEncoderPorts[0],
          ShooterConstants.kEncoderPorts[1],
          ShooterConstants.kEncoderReversed);

  // Create a new SysId routine for characterizing the shooter.
  private final SysIdRoutine m_sysIdRoutine =
      new SysIdRoutine(
          // Empty config defaults to 1 volt/second ramp rate and 7 volt step voltage.
          new SysIdRoutine.Config(),
          new SysIdRoutine.Mechanism(
              // Tell SysId how to plumb the driving voltage to the motor(s).
              m_shooterMotor::setVoltage,
              // Tell SysId how to record a frame of data for each motor on the mechanism being
              // characterized.
              log -> {
                // Record a frame for the shooter motor.
                log.motor("shooter-wheel")
                    .voltage(Volts.of(m_shooterMotor.get() * RobotController.getBatteryVoltage()))
                    .angularPosition(Rotations.of(m_shooterEncoder.getDistance()))
                    .angularVelocity(RotationsPerSecond.of(m_shooterEncoder.getRate()));
              },
              // Tell SysId to make generated commands require this subsystem, suffix test state in
              // WPILog with this subsystem's name ("shooter")
              this));
  // PID controller to run the shooter wheel in closed-loop, set the constants equal to those
  // calculated by SysId
  private final PIDController m_shooterFeedback = new PIDController(ShooterConstants.kP, 0, 0);
  // Feedforward controller to run the shooter wheel in closed-loop, set the constants equal to
  // those calculated by SysId
  private final SimpleMotorFeedforward m_shooterFeedforward =
      new SimpleMotorFeedforward(ShooterConstants.kS, ShooterConstants.kV, ShooterConstants.kA);

  /** Creates a new Shooter subsystem. */
  public Shooter() {
    // Sets the distance per pulse for the encoders
    m_shooterEncoder.setDistancePerPulse(ShooterConstants.kEncoderDistancePerPulse);
  }

  /**
   * Returns a command that runs the shooter at a specifc velocity.
   *
   * @param shooterSpeed The commanded shooter wheel speed in rotations per second
   */
  public Command runShooter(DoubleSupplier shooterSpeed) {
    // Run shooter wheel at the desired speed using a PID controller and feedforward.
    return run(() -> {
          m_shooterMotor.setVoltage(
              m_shooterFeedback.calculate(m_shooterEncoder.getRate(), shooterSpeed.getAsDouble())
                  + m_shooterFeedforward.calculate(shooterSpeed.getAsDouble()));
          m_feederMotor.set(ShooterConstants.kFeederSpeed);
        })
        .finallyDo(
            () -> {
              m_shooterMotor.stopMotor();
              m_feederMotor.stopMotor();
            })
        .withName("runShooter");
  }

  /**
   * Returns a command that will execute a quasistatic test in the given direction.
   *
   * @param direction The direction (forward or reverse) to run the test in
   */
  public Command sysIdQuasistatic(SysIdRoutine.Direction direction) {
    return m_sysIdRoutine.quasistatic(direction);
  }

  /**
   * Returns a command that will execute a dynamic test in the given direction.
   *
   * @param direction The direction (forward or reverse) to run the test in
   */
  public Command sysIdDynamic(SysIdRoutine.Direction direction) {
    return m_sysIdRoutine.dynamic(direction);
  }
}
