// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.async.subsystems;

import static edu.wpi.first.wpilibj.examples.async.Constants.IntakeConstants;
import static edu.wpi.first.wpilibj3.command.async.AsyncCommand.pause;

import edu.wpi.first.wpilibj.DoubleSolenoid;
import edu.wpi.first.wpilibj.PneumaticsModuleType;
import edu.wpi.first.wpilibj.motorcontrol.PWMSparkMax;
import edu.wpi.first.wpilibj3.command.async.AsyncCommand;
import edu.wpi.first.wpilibj3.command.async.HardwareResource;

public class Intake extends HardwareResource {
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
  public AsyncCommand intakeCommand() {
    return run(() -> {
      m_pistons.set(DoubleSolenoid.Value.kForward);
      while (true) {
        pause();
        m_motor.set(1);
      }
    }).named("Intake");
  }

  /** Returns a command that turns off and retracts the intake. */
  public AsyncCommand retractCommand() {
    return run(() -> {
      m_motor.disable();
      m_pistons.set(DoubleSolenoid.Value.kReverse);
    }).named("Retract");
  }
}
