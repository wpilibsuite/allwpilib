// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.gearsbot.subsystems;

import static edu.wpi.first.wpilibj.examples.gearsbot.Constants.ClawConstants.*;

import edu.wpi.first.wpilibj.DigitalInput;
import edu.wpi.first.wpilibj.motorcontrol.Victor;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;
import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.Commands;
import edu.wpi.first.wpilibj2.command.SubsystemBase;

/**
 * The claw subsystem is a simple system with a motor for opening and closing. If using stronger
 * motors, you should probably use a sensor so that the motors don't stall.
 */
public class Claw extends SubsystemBase {
  private final Victor m_motor = new Victor(kMotorPort);
  private final DigitalInput m_contact = new DigitalInput(kContactPort);

  /** Create a new claw subsystem. */
  public Claw() {
    // Let's name everything on the LiveWindow
    addChild("Motor", m_motor);
    addChild("Limit Switch", m_contact);
  }

  public void log() {
    SmartDashboard.putData("Claw switch", m_contact);
  }

  /** Set the claw motor to move in the open direction. */
  public Command open() {
    return runOnce(() -> m_motor.set(-1))
        .andThen(Commands.waitSeconds(1))
        .andThen(m_motor::stopMotor)
        .withName("open claw");
  }

  /** Set the claw motor to move in the close direction. */
  public Command close() {
    return runOnce(() -> m_motor.set(1))
        .andThen(Commands.waitSeconds(1))
        .andThen(m_motor::stopMotor)
        .withName("close claw");
  }

  /** Return true when the robot is grabbing an object hard enough to trigger the limit switch. */
  public boolean isGrabbing() {
    return m_contact.get();
  }

  /** Call log method every loop. */
  @Override
  public void periodic() {
    log();
  }
}
