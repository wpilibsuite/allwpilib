// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.rebuiltcmdv3.mechanisms;

import static org.wpilib.units.Units.RadiansPerSecond;

import org.wpilib.command3.Command;
import org.wpilib.command3.Mechanism;
import org.wpilib.epilogue.Logged;
import org.wpilib.examples.rebuiltcmdv3.constants.IntakeConstants;
import org.wpilib.examples.rebuiltcmdv3.stubs.ExampleSmartMotorController;

@Logged
public class IntakeRoller implements Mechanism {
  private final ExampleSmartMotorController motor =
      new ExampleSmartMotorController(IntakeConstants.ROLLER_MOTOR_ID);

  /**
   * Stops the roller motor. This command keeps the roller off until interrupted by another command.
   *
   * @return Command to stop the roller
   */
  public Command stop() {
    return runRepeatedly(motor::stopMotor).named("Intake.Roller.Stop");
  }

  @Override
  public Command idle() {
    return stop();
  }

  /**
   * Starts the roller motor in intake mode. This command keeps the roller running until interrupted
   * by another command.
   *
   * @return Command to start the roller in intake mode
   */
  public Command intake() {
    return run(coroutine -> {
          motor.setSetpoint(
              ExampleSmartMotorController.PIDMode.kVelocity,
              IntakeConstants.ROLLER_INTAKE_SPEED.in(RadiansPerSecond));

          coroutine.park();
        })
        .named("Intake.Roller.Intake");
  }

  /**
   * Starts the roller motor in expel mode. This command keeps the roller running until interrupted
   * by another command.
   *
   * @return Command to start the roller in expel mode
   */
  public Command expel() {
    return run(coroutine -> {
          motor.setSetpoint(
              ExampleSmartMotorController.PIDMode.kVelocity,
              IntakeConstants.ROLLER_EXPULSION_SPEED.in(RadiansPerSecond));

          coroutine.park();
        })
        .named("Intake.Roller.Expel");
  }
}
