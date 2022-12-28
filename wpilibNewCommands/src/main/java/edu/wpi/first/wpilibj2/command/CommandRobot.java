// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import edu.wpi.first.wpilibj.TimedRobot;

/**
 * The CommandRobot class extends the {@link TimedRobot} class in order to ease functionality for
 * Command Based Programming.
 */
public class CommandRobot extends TimedRobot {
  private static final double kSchedulerOffset = 0.005;

  /**
   * Constructor for CommandRobot.
   *
   * @param mainPeriod periodic rate of the Main Robot Loop in seconds.
   * @param schedulerPeriod periodic rate of the CommandScheduler in seconds.
   */
  public CommandRobot(double mainPeriod, double schedulerPeriod) {
    super(mainPeriod);
    addPeriodic(CommandScheduler.getInstance()::run, schedulerPeriod, kSchedulerOffset);
  }

  /**
   * Constructor for CommandRobot.
   *
   * @param period period to set for both the CommandScheduler and the Main Robot Loop in seconds.
   */
  public CommandRobot(double period) {
    this(period, period);
  }

  /** Set both the CommandScheduler and the Main Robot Loop to run at the default periodic rate. */
  public CommandRobot() {
    this(kDefaultPeriod);
  }
}
