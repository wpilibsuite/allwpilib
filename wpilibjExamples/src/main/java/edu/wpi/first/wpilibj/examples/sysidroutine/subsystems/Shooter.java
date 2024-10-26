// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.sysidroutine.subsystems;

import static edu.wpi.first.units.Units.Radians;
import static edu.wpi.first.units.Units.RadiansPerSecond;
import static edu.wpi.first.units.Units.Rotations;
import static edu.wpi.first.units.Units.RotationsPerSecond;
import static edu.wpi.first.units.Units.Volts;

import edu.wpi.first.math.controller.PIDController;
import edu.wpi.first.math.controller.SimpleMotorFeedforward;
import edu.wpi.first.units.measure.MutAngle;
import edu.wpi.first.units.measure.MutAngularVelocity;
import edu.wpi.first.units.measure.MutVoltage;
import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.RobotController;
import edu.wpi.first.wpilibj.examples.sysidroutine.Constants.ShooterConstants;
import edu.wpi.first.wpilibj.motorcontrol.PWMSparkMax;
import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.SubsystemBase;
import edu.wpi.first.wpilibj2.command.sysid.SysIdRoutine;
import java.util.function.DoubleSupplier;

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

  // Mutable holder for unit-safe voltage values, persisted to avoid reallocation.
  private final MutVoltage m_appliedVoltage = Volts.mutable(0);
  // Mutable holder for unit-safe linear distance values, persisted to avoid reallocation.
  private final MutAngle m_angle = Radians.mutable(0);
  // Mutable holder for unit-safe linear velocity values, persisted to avoid reallocation.
  private final MutAngularVelocity m_velocity = RadiansPerSecond.mutable(0);

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
                    .voltage(
                        m_appliedVoltage.mut_replace(
                            m_shooterMotor.get() * RobotController.getBatteryVoltage(), Volts))
                    .angularPosition(m_angle.mut_replace(m_shooterEncoder.getDistance(), Rotations))
                    .angularVelocity(
                        m_velocity.mut_replace(m_shooterEncoder.getRate(), RotationsPerSecond));
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
      new SimpleMotorFeedforward(
          ShooterConstants.kSVolts,
          ShooterConstants.kVVoltSecondsPerRotation,
          ShooterConstants.kAVoltSecondsSquaredPerRotation);

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
                  + m_shooterFeedforward
                      .calculate(RotationsPerSecond.of(shooterSpeed.getAsDouble()))
                      .in(Volts));
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
