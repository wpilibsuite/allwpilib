// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.async.subsystems;

import edu.wpi.first.wpilibj.DigitalInput;
import edu.wpi.first.wpilibj.examples.async.AsyncCommandBot;
import edu.wpi.first.wpilibj.examples.async.Constants.StorageConstants;
import edu.wpi.first.wpilibj.motorcontrol.PWMSparkMax;
import edu.wpi.first.wpilibj3.command.async.AsyncCommand;
import edu.wpi.first.wpilibj3.command.async.AsyncScheduler;
import edu.wpi.first.wpilibj3.command.async.HardwareResource;
import edu.wpi.first.wpilibj3.command.async.button.AsyncTrigger;

public class Storage extends HardwareResource {
  private final PWMSparkMax m_motor = new PWMSparkMax(StorageConstants.kMotorPort);
  private final DigitalInput m_ballSensor = new DigitalInput(StorageConstants.kBallSensorPort);

  // Expose trigger from subsystem to improve readability and ease
  // inter-subsystem communications
  /** Whether the ball storage is full. */
  @SuppressWarnings("checkstyle:MemberName")
  public final AsyncTrigger hasCargo = new AsyncTrigger(m_ballSensor::get);

  /** Create a new Storage subsystem. */
  public Storage() {
    super("Storage");

    // Set default command to turn off the storage motor and then idle
    AsyncScheduler.getInstance().setDefaultCommand(this, run(() -> {
      m_motor.disable();
      Thread.sleep(Long.MAX_VALUE);
    }).named("Idle"));
  }

  public void run() throws InterruptedException {
    m_motor.set(1);
  }

  /** Returns a command that runs the storage motor indefinitely. */
  public AsyncCommand runCommand() {
    return run(() -> {
      while (true) {
        AsyncCommand.pause();
        run();
      }
    }).named("Run");
  }
}
