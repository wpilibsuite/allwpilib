// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.rapidreactcommandbot.subsystems;

import static org.wpilib.examples.rapidreactcommandbot.Constants.IntakeConstants;

import org.wpilib.command2.Command;
import org.wpilib.command2.SubsystemBase;
import org.wpilib.epilogue.Logged;
import org.wpilib.examples.rapidreactcommandbot.Constants.IntakeConstants;
import org.wpilib.hardware.motor.PWMSparkMax;
import org.wpilib.hardware.pneumatic.DoubleSolenoid;
import org.wpilib.hardware.pneumatic.PneumaticsModuleType;

@Logged
public class Intake extends SubsystemBase {
  private final PWMSparkMax m_motor = new PWMSparkMax(IntakeConstants.kMotorPort);

  // Double solenoid connected to two channels of a PCM with the default CAN ID
  private final DoubleSolenoid m_pistons =
      new DoubleSolenoid(
          0,
          PneumaticsModuleType.CTREPCM,
          IntakeConstants.kSolenoidPorts[0],
          IntakeConstants.kSolenoidPorts[1]);

  /** Returns a command that deploys the intake, and then runs the intake motor indefinitely. */
  public Command intakeCommand() {
    return runOnce(() -> m_pistons.set(DoubleSolenoid.Value.kForward))
        .andThen(run(() -> m_motor.setDutyCycle(1.0)))
        .withName("Intake");
  }

  /** Returns a command that turns off and retracts the intake. */
  public Command retractCommand() {
    return runOnce(
            () -> {
              m_motor.disable();
              m_pistons.set(DoubleSolenoid.Value.kReverse);
            })
        .withName("Retract");
  }
}
