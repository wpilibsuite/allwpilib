/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.buttons;

import edu.wpi.first.wpilibj.XboxController;

/**
 * A {@link Button} that gets its state from an {@link XboxController}.
 */
public class XboxControllerButton extends Button {
  /**
   * Represents a digital button on an XboxController.
   */
  public enum Button {
    kBumperLeft(5),
    kBumperRight(6),
    kStickLeft(9),
    kStickRight(10),
    kA(1),
    kB(2),
    kX(3),
    kY(4),
    kBack(7),
    kStart(8);

    @SuppressWarnings("MemberName")
    private int value;

    Button(int value) {
      this.value = value;
    }
  }

  private final XboxController m_joystick;
  private final int m_buttonNumber;

  /**
   * Create a joystick button for triggering commands.
   *
   * @param joystick     The XboxController object that has the button
   * @param button The button to use (see {@link XboxControllerButton.Button}
   */
  public XboxControllerButton(XboxController joystick, XboxControllerButton.Button button) {
    m_joystick = joystick;
    m_buttonNumber = button.value;
  }

  /**
   * Gets the value of the joystick button.
   *
   * @return The value of the joystick button
   */
  public boolean get() {
    return m_joystick.getRawButton(m_buttonNumber);
  }
}
