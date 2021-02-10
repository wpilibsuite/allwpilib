// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.pacgoat.commands;

import edu.wpi.first.wpilibj.command.InstantCommand;
import edu.wpi.first.wpilibj.examples.pacgoat.Robot;

/**
 * This command sets the collector rollers spinning at the given speed. Since there is no sensor for
 * detecting speed, it finishes immediately. As a result, the spinners may still be adjusting their
 * speed.
 */
public class SetCollectionSpeed extends InstantCommand {
  private final double m_speed;

  public SetCollectionSpeed(double speed) {
    requires(Robot.collector);
    this.m_speed = speed;
  }

  // Called just before this Command runs the first time
  @Override
  protected void initialize() {
    Robot.collector.setSpeed(m_speed);
  }
}
