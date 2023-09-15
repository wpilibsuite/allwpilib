// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.rapidreactcommandbot.subsystems;

import static edu.wpi.first.wpilibj.examples.rapidreactcommandbot.Constants.StorageConstants;

import edu.wpi.first.command2.Command;
import edu.wpi.first.command2.SubsystemBase;
import edu.wpi.first.wpilibj.DigitalInput;
import edu.wpi.first.wpilibj.motorcontrol.PWMSparkMax;

public class Storage extends SubsystemBase {
  private final PWMSparkMax m_motor = new PWMSparkMax(StorageConstants.kMotorPort);
  private final DigitalInput m_ballSensor = new DigitalInput(StorageConstants.kBallSensorPort);

  /** Create a new Storage subsystem. */
  public Storage() {
    // Set default command to turn off the storage motor and then idle
    setDefaultCommand(runOnce(m_motor::disable).andThen(run(() -> {})).withName("Idle"));
  }

  /** Whether the ball storage is full. */
  public boolean isFull() {
    return m_ballSensor.get();
  }

  /** Returns a command that runs the storage motor indefinitely. */
  public Command runCommand() {
    return run(() -> m_motor.set(1)).withName("run");
  }
}
