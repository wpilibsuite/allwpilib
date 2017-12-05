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
  private enum Button {
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

  private final XboxController m_controller;
  private final int m_buttonNumber;

  /**
   * Create an Xbox Controller button for triggering commands.
   *
   * @param controller   The XboxController object that has the button
   * @param button       The button to use (see {@link XboxControllerButton.Button}
   */
  private XboxControllerButton(XboxController controller, XboxControllerButton.Button button) {
    m_controller = controller;
    m_buttonNumber = button.value;
  }

  /**
   * Gets the value of the controller button.
   *
   * @return The value of the controller button
   */
  public boolean get() {
    return m_controller.getRawButton(m_buttonNumber);
  }

  /**
   * TODO: Finish javadocs.
   * TODO: Use Hand for left/right, possibly.
   */
  public static XboxControllerButton getBumperLeftButton(XboxController controller) {
    return new XboxControllerButton(controller, Button.kBumperLeft);
  }

  public static XboxControllerButton getBumperRightButton(XboxController controller) {
    return new XboxControllerButton(controller, Button.kBumperRight);
  }

  public static XboxControllerButton getStickLeftButton(XboxController controller) {
    return new XboxControllerButton(controller, Button.kStickLeft);
  }

  public static XboxControllerButton getStickRightButton(XboxController controller) {
    return new XboxControllerButton(controller, Button.kStickRight);
  }

  public static XboxControllerButton getAButton(XboxController controller) {
    return new XboxControllerButton(controller, Button.kA);
  }

  public static XboxControllerButton getBButton(XboxController controller) {
    return new XboxControllerButton(controller, Button.kB);
  }

  public static XboxControllerButton getXButton(XboxController controller) {
    return new XboxControllerButton(controller, Button.kX);
  }

  public static XboxControllerButton getYButton(XboxController controller) {
    return new XboxControllerButton(controller, Button.kY);
  }

  public static XboxControllerButton getBackButton(XboxController controller) {
    return new XboxControllerButton(controller, Button.kBack);
  }

  public static XboxControllerButton getStartButton(XboxController controller) {
    return new XboxControllerButton(controller, Button.kStart);
  }
}
