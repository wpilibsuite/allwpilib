// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.pacgoat.triggers;

import edu.wpi.first.wpilibj.GenericHID;
import edu.wpi.first.wpilibj.buttons.Trigger;

/** A custom button that is triggered when TWO buttons on a Joystick are simultaneously pressed. */
public class DoubleButton extends Trigger {
  private final GenericHID m_joy;
  private final int m_button1;
  private final int m_button2;

  /**
   * Create a new double button trigger.
   *
   * @param joy The joystick
   * @param button1 The first button
   * @param button2 The second button
   */
  public DoubleButton(GenericHID joy, int button1, int button2) {
    this.m_joy = joy;
    this.m_button1 = button1;
    this.m_button2 = button2;
  }

  @Override
  public boolean get() {
    return m_joy.getRawButton(m_button1) && m_joy.getRawButton(m_button2);
  }
}
