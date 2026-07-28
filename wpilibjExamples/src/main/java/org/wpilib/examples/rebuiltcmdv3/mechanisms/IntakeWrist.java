// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.rebuiltcmdv3.mechanisms;

import static org.wpilib.units.Units.Degrees;
import static org.wpilib.units.Units.Radians;

import org.wpilib.command3.Command;
import org.wpilib.command3.Mechanism;
import org.wpilib.epilogue.Logged;
import org.wpilib.examples.rebuiltcmdv3.constants.IntakeConstants;
import org.wpilib.examples.rebuiltcmdv3.stubs.ExampleSmartMotorController;
import org.wpilib.units.measure.Angle;

@Logged
public class IntakeWrist implements Mechanism {
  private final ExampleSmartMotorController motor =
      new ExampleSmartMotorController(IntakeConstants.WRIST_MOTOR_ID);

  public Command stop() {
    return runRepeatedly(motor::stopMotor).named("Intake.Wrist.Stop");
  }

  @Override
  public Command idle() {
    return stop();
  }

  /**
   * Moves the wrist to a specific angle. The command will exit when the target angle is reached.
   *
   * @param target The target angle to move to
   * @return A command that moves the wrist to the target angle
   */
  public Command moveToAngle(Angle target) {
    double targetRads = target.in(Radians);
    double tolerance = Radians.convertFrom(0.5, Degrees);

    return run(coroutine -> {
          motor.setSetpoint(ExampleSmartMotorController.PIDMode.kPosition, targetRads);

          coroutine.waitUntil(() -> Math.abs(motor.getEncoderDistance() - targetRads) <= tolerance);
        })
        .named("Intake.Wrist.MoveToAngle[" + target.toLongString() + "]");
  }

  /**
   * Moves the wrist to a specific angle. The command will hold the wrist at the target angle until
   * interrupted.
   *
   * @param target The target angle to hold
   * @return A command that holds the wrist at the target angle
   */
  public Command holdAngle(Angle target) {
    double targetRads = target.in(Radians);

    return run(coroutine -> {
          motor.setSetpoint(ExampleSmartMotorController.PIDMode.kPosition, targetRads);
          coroutine.park();
        })
        .named("Intake.Wrist.HoldAngle[" + target.toLongString() + "]");
  }

  /**
   * Moves the wrist to the stowed position and holds it there until interrupted.
   *
   * @return A command that stows the wrist
   */
  public Command stow() {
    return holdAngle(IntakeConstants.WRIST_STOW_ANGLE);
  }

  /**
   * Moves the wrist to the down position and holds it there until interrupted.
   *
   * @return A command that moves the wrist to the down position
   */
  public Command down() {
    return holdAngle(IntakeConstants.WRIST_DOWN_ANGLE);
  }
}
