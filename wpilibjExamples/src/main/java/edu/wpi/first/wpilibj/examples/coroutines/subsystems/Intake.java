// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.coroutines.subsystems;

import static edu.wpi.first.wpilibj.examples.coroutines.Constants.IntakeConstants;

import edu.wpi.first.wpilibj.DoubleSolenoid;
import edu.wpi.first.wpilibj.PneumaticsModuleType;
import org.wpilib.commands3.Command;
import org.wpilib.commands3.RequireableResource;
import edu.wpi.first.wpilibj.motorcontrol.PWMSparkMax;

public class Intake extends RequireableResource {
  private final PWMSparkMax m_motor = new PWMSparkMax(IntakeConstants.kMotorPort);

  // Double solenoid connected to two channels of a PCM with the default CAN ID
  private final DoubleSolenoid m_pistons =
      new DoubleSolenoid(
          PneumaticsModuleType.CTREPCM,
          IntakeConstants.kSolenoidPorts[0],
          IntakeConstants.kSolenoidPorts[1]);

  public Intake() {
    super("Intake");
  }

  /** Returns a command that deploys the intake, and then runs the intake motor indefinitely. */
  public Command intakeCommand() {
    return run((coroutine) -> {
      m_pistons.set(DoubleSolenoid.Value.kForward);
      while (coroutine.yield()) {
        m_motor.set(1);
      }
    }).named("Intake");
  }

  /** Returns a command that turns off and retracts the intake. */
  public Command retractCommand() {
    return run((coroutine) -> {
      m_motor.disable();
      m_pistons.set(DoubleSolenoid.Value.kReverse);
    }).named("Retract");
  }
}
