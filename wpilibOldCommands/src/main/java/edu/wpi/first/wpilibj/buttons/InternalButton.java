// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.buttons;

/**
 * This class is intended to be used within a program. The programmer can manually set its value.
 * Also includes a setting for whether or not it should invert its value.
 */
public class InternalButton extends Button {
  private boolean m_pressed;
  private boolean m_inverted;

  /**
   * Creates an InternalButton that is not inverted.
   */
  public InternalButton() {
    this(false);
  }

  /**
   * Creates an InternalButton which is inverted depending on the input.
   *
   * @param inverted if false, then this button is pressed when set to true, otherwise it is pressed
   *                 when set to false.
   */
  public InternalButton(boolean inverted) {
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
