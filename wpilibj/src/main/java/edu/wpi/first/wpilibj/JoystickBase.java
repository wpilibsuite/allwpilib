/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

/**
 * JoystickBase Interface.
 */
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

  /**
   * Is the trigger pressed.
   *
   * @return true if pressed
   */
  public final boolean getTrigger() {
    return getTrigger(Hand.kRight);
  }

  /**
   * Is the trigger pressed.
   *
   * @param hand which hand
   * @return true if the trigger for the given hand is pressed
   */
  public abstract boolean getTrigger(Hand hand);

  /**
   * Is the top button pressed.
   *
   * @return true if the top button is pressed
   */
  public final boolean getTop() {
    return getTop(Hand.kRight);
  }

  /**
   * Is the top button pressed.
   *
   * @param hand which hand
   * @return true if hte top button for the given hand is pressed
   */
  public abstract boolean getTop(Hand hand);

  public abstract int getPOV(int pov);

  public abstract int getPOVCount();

  public abstract HIDType getType();

  public abstract String getName();

  public abstract void setOutput(int outputNumber, boolean value);

  public abstract void setOutputs(int value);

  public abstract void setRumble(RumbleType type, double value);
}
