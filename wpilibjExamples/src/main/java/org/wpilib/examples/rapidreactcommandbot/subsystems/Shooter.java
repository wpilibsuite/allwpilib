// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.rapidreactcommandbot.subsystems;

import static org.wpilib.command2.Commands.parallel;
import static org.wpilib.command2.Commands.waitUntil;

import org.wpilib.command2.Command;
import org.wpilib.command2.SubsystemBase;
import org.wpilib.drivers.motor.PWMSparkMax;
import org.wpilib.epilogue.Logged;
import org.wpilib.examples.rapidreactcommandbot.Constants.ShooterConstants;
import org.wpilib.hardware.rotation.Encoder;
import org.wpilib.math.controller.PIDController;
import org.wpilib.math.controller.SimpleMotorFeedforward;

@Logged
public class Shooter extends SubsystemBase {
  private final PWMSparkMax shooterMotor = new PWMSparkMax(ShooterConstants.SHOOTER_MOTOR_PORT);
  private final PWMSparkMax feederMotor = new PWMSparkMax(ShooterConstants.FEEDER_MOTOR_PORT);
  private final Encoder shooterEncoder =
      new Encoder(
          ShooterConstants.ENCODER_PORTS[0],
          ShooterConstants.ENCODER_PORTS[1],
          ShooterConstants.ENCODER_REVERSED);
  private final SimpleMotorFeedforward shooterFeedforward =
      new SimpleMotorFeedforward(ShooterConstants.kS, ShooterConstants.kV);
  private final PIDController shooterFeedback = new PIDController(ShooterConstants.kP, 0.0, 0.0);

  /** The shooter subsystem for the robot. */
  public Shooter() {
    shooterFeedback.setTolerance(ShooterConstants.SHOOTER_TOLERANCE_RPS);
    shooterEncoder.setDistancePerPulse(ShooterConstants.ENCODER_DISTANCE_PER_PULSE);

    // Set default command to turn off both the shooter and feeder motors, and then idle
    setDefaultCommand(
        runOnce(
                () -> {
                  shooterMotor.setThrottle(0.0);
                  feederMotor.setThrottle(0.0);
                })
            .andThen(run(() -> {}))
            .withName("Idle"));
  }

  /**
   * Returns a command to shoot the balls currently stored in the robot. Spins the shooter flywheel
   * up to the specified setpoint, and then runs the feeder motor.
   *
   * @param setpointRotationsPerSecond The desired shooter velocity
   */
  public Command shootCommand(double setpointRotationsPerSecond) {
    return parallel(
            // Run the shooter flywheel at the desired setpoint using feedforward and feedback
            run(
                () -> {
                  shooterMotor.setThrottle(
                      shooterFeedforward.calculate(setpointRotationsPerSecond)
                          + shooterFeedback.calculate(
                              shooterEncoder.getRate(), setpointRotationsPerSecond));
                }),

            // Wait until the shooter has reached the setpoint, and then run the feeder
            waitUntil(shooterFeedback::atSetpoint).andThen(() -> feederMotor.setThrottle(1)))
        .withName("Shoot");
  }
}
