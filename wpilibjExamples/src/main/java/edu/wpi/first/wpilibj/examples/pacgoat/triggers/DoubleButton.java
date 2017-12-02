/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.pacgoat.triggers;

import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.buttons.Trigger;

/**
 * A custom button that is triggered when two buttons on a Joystick are
 * simultaneously pressed.
 */
public class DoubleButton extends Trigger {
  private final Joystick m_joy;
  private final int m_button1;
  private final int m_button2;
  private boolean m_button1Pressed;
  private boolean m_button2Pressed;
  private boolean m_button1Released;
  private boolean m_button2Released;

  /**
   * Create a new double button trigger.
   * @param joy The joystick
   * @param button1 The first button
   * @param button2 The second button
   */
  public DoubleButton(Joystick joy, int button1, int button2) {
    this.m_joy = joy;
    this.m_button1 = button1;
    this.m_button2 = button2;
  }

  @Override
  public boolean get() {
    return m_joy.getRawButton(m_button1) && m_joy.getRawButton(m_button2);
  }

  @Override
  public boolean getPressed() {
    m_button1Pressed |= m_joy.getRawButtonPressed(m_button1);
    m_button2Pressed |= m_joy.getRawButtonPressed(m_button2);
    if (m_button1Pressed && m_button2Pressed) {
      m_button1Pressed = false;
      m_button2Pressed = false;
      return true;
    } else {
      return false;
    }
  }

  @Override
  public boolean getReleased() {
    m_button1Released |= m_joy.getRawButtonReleased(m_button1);
    m_button2Released |= m_joy.getRawButtonReleased(m_button2);
    if (m_button1Released && m_button2Released) {
      m_button1Released = false;
      m_button2Released = false;
      return true;
    } else {
      return false;
    }
  }
}
