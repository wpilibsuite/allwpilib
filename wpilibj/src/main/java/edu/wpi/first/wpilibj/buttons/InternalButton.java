/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.buttons;

/**
 * This class is intended to be used within a program. The programmer can manually set its value.
 * Also includes a setting for whether or not it should invert its value.
 */
public class InternalButton extends Button {
  private boolean m_state;
  private boolean m_lastState;
  private boolean m_pressed;
  private boolean m_released;
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
    m_state = m_lastState = m_inverted = inverted;
  }

  public void setInverted(boolean inverted) {
    m_inverted = inverted;
  }

  /**
   * Set current state of button.
   *
   * @param pressed Whether the button is pressed.
   */
  public void setPressed(boolean pressed) {
    m_lastState = m_state;
    m_state = pressed;
    m_pressed |= !(m_lastState ^ m_inverted) && (m_state ^ m_inverted);
    m_released |= (m_lastState ^ m_inverted) && !(m_state ^ m_inverted);
  }

  @Override
  public boolean get() {
    return m_state ^ m_inverted;
  }

  @Override
  public boolean getPressed() {
    m_pressed |= !(m_lastState ^ m_inverted) && (m_state ^ m_inverted);
    m_released |= (m_lastState ^ m_inverted) && !(m_state ^ m_inverted);
    m_lastState = m_state;

    if (m_pressed) {
      m_pressed = false;
      return true;
    } else {
      return false;
    }
  }

  @Override
  public boolean getReleased() {
    m_pressed |= !(m_lastState ^ m_inverted) && (m_state ^ m_inverted);
    m_released |= (m_lastState ^ m_inverted) && !(m_state ^ m_inverted);
    m_lastState = m_state;

    if (m_released) {
      m_released = false;
      return true;
    } else {
      return false;
    }
  }
}
