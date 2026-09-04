// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.rebuiltcmdv3.mechanisms;

import static org.wpilib.units.Units.RPM;

import java.util.function.Supplier;
import org.wpilib.command3.Command;
import org.wpilib.command3.Mechanism;
import org.wpilib.epilogue.Logged;
import org.wpilib.examples.rebuiltcmdv3.constants.ShooterConstants;
import org.wpilib.examples.rebuiltcmdv3.stubs.ExampleSmartMotorController;
import org.wpilib.units.measure.AngularVelocity;

/** The flywheel for the shooter. The flywheel spins up to shoot the ball. */
@Logged
public class ShooterFlywheel implements Mechanism {
  private final ExampleSmartMotorController flywheelMotorPrimary;
  private final ExampleSmartMotorController flywheelMotorSecondary;
  private final ExampleSmartMotorController flywheelMotorTertiary;
  private final ExampleSmartMotorController flywheelMotorQuatenary;

  /**
   * Creates a new shooter flywheel. This should only be used by the {@link Shooter} mechanism, not
   * the central Robot class.
   */
  public ShooterFlywheel() {
    flywheelMotorPrimary =
        new ExampleSmartMotorController(ShooterConstants.PRIMARY_SHOOTER_MOTOR_ID);
    flywheelMotorSecondary =
        new ExampleSmartMotorController(ShooterConstants.SECONDARY_SHOOTER_MOTOR_ID);
    flywheelMotorTertiary =
        new ExampleSmartMotorController(ShooterConstants.TERTIARY_SHOOTER_MOTOR_ID);
    flywheelMotorQuatenary =
        new ExampleSmartMotorController(ShooterConstants.QUATERNARY_SHOOTER_MOTOR_ID);

    flywheelMotorSecondary.follow(flywheelMotorPrimary);
    flywheelMotorTertiary.follow(flywheelMotorPrimary);
    flywheelMotorQuatenary.follow(flywheelMotorPrimary);

    flywheelMotorPrimary.setPID(ShooterConstants.FLYWHEEL_KP, 0, 0);
  }

  @Override
  public Command idle() {
    return runRepeatedly(flywheelMotorPrimary::stopMotor).named("ShooterFlywheel.Idle");
  }

  /**
   * Runs the flywheel at the specified speed and holds it. This command will run forever unless
   * interrupted.
   *
   * @param speed A dynamic supplier for the desired flywheel speed.
   */
  public Command runFlywheelSpeed(Supplier<AngularVelocity> speed) {
    return runRepeatedly(
            () -> {
              flywheelMotorPrimary.setSetpoint(
                  ExampleSmartMotorController.PIDMode.VELOCITY, speed.get().in(RPM));
            })
        .named("Shooter.Flywheel.RunFlywheelSpeed");
  }
}
