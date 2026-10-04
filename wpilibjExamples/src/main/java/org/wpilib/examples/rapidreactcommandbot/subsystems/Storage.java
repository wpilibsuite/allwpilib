// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.rapidreactcommandbot.subsystems;

import module wpilib;
import module wpilib.command2;
import module wpilib.drivers;

import org.wpilib.examples.rapidreactcommandbot.Constants.StorageConstants;

@Logged
public class Storage extends SubsystemBase {
  private final PWMSparkMax motor = new PWMSparkMax(StorageConstants.MOTOR_PORT);
  @NotLogged // We'll log a more meaningful boolean instead
  private final DigitalInput ballSensor = new DigitalInput(StorageConstants.BALL_SENSOR_PORT);

  // Expose trigger from subsystem to improve readability and ease
  // inter-subsystem communications
  /** Whether the ball storage is full. */
  @Logged(name = "Has Cargo")
  public final Trigger hasCargo = new Trigger(ballSensor::get);

  /** Create a new Storage subsystem. */
  public Storage() {
    // Set default command to turn off the storage motor and then idle
    setDefaultCommand(runOnce(() -> motor.setThrottle(0)).andThen(run(() -> {})).withName("Idle"));
  }

  /** Returns a command that runs the storage motor indefinitely. */
  public Command runCommand() {
    return run(() -> motor.setThrottle(1)).withName("run");
  }
}
