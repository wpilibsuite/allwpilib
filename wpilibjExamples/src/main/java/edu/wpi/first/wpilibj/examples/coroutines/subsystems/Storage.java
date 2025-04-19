// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.coroutines.subsystems;

import edu.wpi.first.wpilibj.DigitalInput;
import edu.wpi.first.wpilibj.commandsv3.Command;
import edu.wpi.first.wpilibj.commandsv3.RequireableResource;
import edu.wpi.first.wpilibj.commandsv3.button.Trigger;
import edu.wpi.first.wpilibj.examples.coroutines.Constants.StorageConstants;
import edu.wpi.first.wpilibj.motorcontrol.PWMSparkMax;

public class Storage extends RequireableResource {
  private final PWMSparkMax m_motor = new PWMSparkMax(StorageConstants.kMotorPort);
  private final DigitalInput m_ballSensor = new DigitalInput(StorageConstants.kBallSensorPort);

  // Expose trigger from subsystem to improve readability and ease
  // inter-subsystem communications
  /** Whether the ball storage is full. */
  @SuppressWarnings("checkstyle:MemberName")
  public final Trigger hasCargo = new Trigger(m_ballSensor::get);

  /** Create a new Storage subsystem. */
  public Storage() {
    super("Storage");

    // Set default command to turn off the storage motor and then idle
    setDefaultCommand(run((coroutine) -> {
      m_motor.disable();
      coroutine.park();
    }).named("Idle"));
  }

  public void run() {
    m_motor.set(1);
  }

  /** Returns a command that runs the storage motor indefinitely. */
  public Command runCommand() {
    return run((coroutine) -> {
      while (coroutine.yield()) {
        run();
      }
    }).named("Run");
  }
}
