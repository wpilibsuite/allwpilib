/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.buttons;

/**
 * This class is intended to be used within a program. The programmer can
 * manually set its value. Also includes a setting for whether or not it should
 * invert its value.
 *
 * @author Joe
 */
public class InternalButton extends Button {

  boolean pressed;
  boolean inverted;

  /**
   * Creates an InternalButton that is not inverted.
   */
  public InternalButton() {
    this(false);
  }

  /**
   * Creates an InternalButton which is inverted depending on the input.
   *
   * @param inverted if false, then this button is pressed when set to true,
   *        otherwise it is pressed when set to false.
   */
  public InternalButton(boolean inverted) {
    this.pressed = this.inverted = inverted;
  }

  public void setInverted(boolean inverted) {
    this.inverted = inverted;
  }

  public void setPressed(boolean pressed) {
    this.pressed = pressed;
  }

  public boolean get() {
    return pressed ^ inverted;
  }
}
