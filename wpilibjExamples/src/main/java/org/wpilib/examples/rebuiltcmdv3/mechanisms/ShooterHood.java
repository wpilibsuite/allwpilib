// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.rebuiltcmdv3.mechanisms;

import static org.wpilib.units.Units.Radians;

import java.util.function.Supplier;
import org.wpilib.command3.Command;
import org.wpilib.command3.Mechanism;
import org.wpilib.epilogue.Logged;
import org.wpilib.examples.rebuiltcmdv3.constants.ShooterConstants;
import org.wpilib.examples.rebuiltcmdv3.stubs.ExampleSmartMotorController;
import org.wpilib.units.measure.Angle;

/** The hood for the shooter. The hood moves up and down to adjust the trajectory of the shots. */
@Logged
public class ShooterHood implements Mechanism {
  private final ExampleSmartMotorController hoodMotor;

  /**
   * Creates a new shooter hood. This should only be used by the {@link Shooter} mechanism, not the
   * central Robot class.
   */
  public ShooterHood() {
    hoodMotor = new ExampleSmartMotorController(ShooterConstants.HOOD_MOTOR_ID);

    hoodMotor.setPID(ShooterConstants.HOOD_KP, 0, 0);
  }

  @Override
  public Command idle() {
    return runRepeatedly(hoodMotor::stopMotor).named("ShooterHood.Idle");
  }

  /**
   * Moves the hood to the specified angle and holds it. This command will run forever unless
   * interrupted.
   *
   * @param angle A dynamic supplier for the desired hood angle.
   */
  public Command runHoodAngle(Supplier<Angle> angle) {
    return runRepeatedly(
            () -> {
              hoodMotor.setSetpoint(
                  ExampleSmartMotorController.PIDMode.POSITION, angle.get().in(Radians));
            })
        .named("Shooter.Hood.RunHoodAngle");
  }
}
