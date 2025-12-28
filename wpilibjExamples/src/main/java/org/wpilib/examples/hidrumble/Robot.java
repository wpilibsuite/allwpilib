// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.hidrumble;

import org.wpilib.driverstation.Gamepad;
import org.wpilib.driverstation.GenericHID.RumbleType;
import org.wpilib.framework.TimedRobot;

/** This is a demo program showing the use of GenericHID's rumble feature. */
public class Robot extends TimedRobot {
  private final Gamepad m_hid = new Gamepad(0);

  @Override
  public void autonomousInit() {
    // Turn on rumble at the start of auto
    m_hid.setRumble(RumbleType.kLeftRumble, 1.0);
    m_hid.setRumble(RumbleType.kRightRumble, 1.0);
  }

  @Override
  public void disabledInit() {
    // Stop the rumble when entering disabled
    m_hid.setRumble(RumbleType.kLeftRumble, 0.0);
    m_hid.setRumble(RumbleType.kRightRumble, 0.0);
  }
}
