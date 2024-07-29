// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.rapidreactcommandbot.subsystems;

import edu.wpi.first.epilogue.Logged;
import edu.wpi.first.epilogue.NotLogged;
import edu.wpi.first.wpilibj.DigitalInput;
import edu.wpi.first.wpilibj.examples.rapidreactcommandbot.Constants.StorageConstants;
import edu.wpi.first.wpilibj.motorcontrol.PWMSparkMax;
import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.SubsystemBase;
import edu.wpi.first.wpilibj2.command.button.Trigger;

@Logged
public class Storage extends SubsystemBase {
  private final PWMSparkMax m_motor = new PWMSparkMax(StorageConstants.kMotorPort);
  @NotLogged // We'll log a more meaningful boolean instead
  private final DigitalInput m_ballSensor = new DigitalInput(StorageConstants.kBallSensorPort);

  // Expose trigger from subsystem to improve readability and ease
  // inter-subsystem communications
  /** Whether the ball storage is full. */
  @Logged(name = "Has Cargo")
  @SuppressWarnings("checkstyle:MemberName")
  public final Trigger hasCargo = new Trigger(m_ballSensor::get);

  /** Create a new Storage subsystem. */
  public Storage() {
    // Set default command to turn off the storage motor and then idle
    setDefaultCommand(runOnce(m_motor::disable).andThen(run(() -> {})).withName("Idle"));
  }

  /** Returns a command that runs the storage motor indefinitely. */
  public Command runCommand() {
    return run(() -> m_motor.set(1)).withName("run");
  }
}
