/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj2.command.button;

import edu.wpi.first.wpilibj.GenericHID;

import static edu.wpi.first.wpilibj.util.ErrorMessages.requireNonNullParam;

/**
 * A {@link Button} that gets its state from a POV on a {@link GenericHID}.
 */
public class POVButton extends Button {
  private final GenericHID m_joystick;
  private final int m_angle;
  private final int m_povNumber;

  /**
   * Creates a POV button for triggering commands.
   *
   * @param joystick The GenericHID object that has the POV
   * @param angle The desired angle in degrees (e.g. 90, 270)
   * @param povNumber The POV number (see {@link GenericHID#getPOV(int)})
   */
  public POVButton(GenericHID joystick, int angle, int povNumber) {
    requireNonNullParam(joystick, "joystick", "POVButton");

    m_joystick = joystick;
    m_angle = angle;
    m_povNumber = povNumber;
  }

  /**
   * Creates a POV button for triggering commands.
   * By default, acts on POV 0
   *
   * @param joystick The GenericHID object that has the POV
   * @param angle The desired angle (e.g. 90, 270)
   */
  public POVButton(GenericHID joystick, int angle) {
    this(joystick, angle, 0);
  }

  /**
   * Checks whether the current value of the POV is the target angle.
   *
   * @return Whether the value of the POV matches the target angle
   */
  @Override
  public boolean get() {
    return m_joystick.getPOV(m_povNumber) == m_angle;
  }
}
