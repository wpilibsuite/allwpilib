// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.rapidreactcommandbot.subsystems;

import edu.wpi.first.epilogue.Logged;
import org.wpilib.DoubleSolenoid;
import org.wpilib.PneumaticsModuleType;
import org.wpilib.examples.rapidreactcommandbot.Constants.IntakeConstants;
import org.wpilib.motorcontrol.PWMSparkMax;
import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.SubsystemBase;

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
        .andThen(run(() -> m_motor.set(1.0)))
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
