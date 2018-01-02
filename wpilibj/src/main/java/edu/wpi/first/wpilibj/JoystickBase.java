/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

/**
 * JoystickBase Interface.
 *
 * @deprecated Inherit directly from GenericHID instead.
 */
@Deprecated
public abstract class JoystickBase extends GenericHID {
  public JoystickBase(int port) {
    super(port);
  }

  /**
   * Get the z position of the HID.
   *
   * @param hand which hand, left or right
   * @return the z position
   */
  public abstract double getZ(Hand hand);

  public double getZ() {
    return getZ(Hand.kRight);
  }

  /**
   * Get the twist value.
   *
   * @return the twist value
   */
  public abstract double getTwist();

  /**
   * Get the throttle.
   *
   * @return the throttle value
   */
  public abstract double getThrottle();
}
