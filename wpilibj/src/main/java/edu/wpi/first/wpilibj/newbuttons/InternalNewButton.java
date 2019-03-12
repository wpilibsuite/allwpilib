/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.newbuttons;

/**
 * This class is intended to be used within a program. The programmer can manually set its value.
 * Also includes a setting for whether or not it should invert its value.
 */
public class InternalNewButton extends NewButton {
  private boolean m_pressed;
  private boolean m_inverted;

  /**
   * Creates an InternalButton that is not inverted.
   */
  public InternalNewButton() {
    this(false);
  }

  /**
   * Creates an InternalButton which is inverted depending on the input.
   *
   * @param inverted if false, then this button is pressed when set to true, otherwise it is pressed
   *                 when set to false.
   */
  public InternalNewButton(boolean inverted) {
    m_pressed = m_inverted = inverted;
  }

  public void setInverted(boolean inverted) {
    m_inverted = inverted;
  }

  public void setPressed(boolean pressed) {
    m_pressed = pressed;
  }

  @Override
  public boolean get() {
    return m_pressed ^ m_inverted;
  }
}
