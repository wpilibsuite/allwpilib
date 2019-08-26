/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.periodvariantflywheel;

import edu.wpi.first.wpilibj.TimedRobot;

import edu.wpi.first.wpilibj.examples.periodvariantflywheel.subsystems.Flywheel;

public class Robot extends TimedRobot {
  public static Flywheel m_flywheel;

  @Override
  public void robotInit() {
    m_flywheel = new Flywheel();
  }
}
