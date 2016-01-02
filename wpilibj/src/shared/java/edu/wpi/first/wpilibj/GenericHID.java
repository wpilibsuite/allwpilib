/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

/**
 * GenericHID Interface
 */
public abstract class GenericHID {

  /**
   * Which hand the Human Interface Device is associated with.
   */
  public static class Hand {

    /**
     * The integer value representing this enumeration
     */
    public final int value;
    static final int kLeft_val = 0;
    static final int kRight_val = 1;
    /**
     * hand: left
     */
    public static final Hand kLeft = new Hand(kLeft_val);
    /**
     * hand: right
     */
    public static final Hand kRight = new Hand(kRight_val);

    private Hand(int value) {
      this.value = value;
    }
  }

  /**
   * Get the x position of the HID
   *$
   * @return the x position of the HID
   */
  public final double getX() {
    return getX(Hand.kRight);
  }

  /**
   * Get the x position of HID
   *$
   * @param hand which hand, left or right
   * @return the x position
   */
  public abstract double getX(Hand hand);

  /**
   * Get the y position of the HID
   *$
   * @return the y position
   */
  public final double getY() {
    return getY(Hand.kRight);
  }

  /**
   * Get the y position of the HID
   *$
   * @param hand which hand, left or right
   * @return the y position
   */
  public abstract double getY(Hand hand);

  /**
   * Get the z position of the HID
   *$
   * @return the z position
   */
  public final double getZ() {
    return getZ(Hand.kRight);
  }

  /**
   * Get the z position of the HID
   *$
   * @param hand which hand, left or right
   * @return the z position
   */
  public abstract double getZ(Hand hand);

  /**
   * Get the twist value
   *$
   * @return the twist value
   */
  public abstract double getTwist();

  /**
   * Get the throttle
   *$
   * @return the throttle value
   */
  public abstract double getThrottle();

  /**
   * Get the raw axis
   *$
   * @param which index of the axis
   * @return the raw value of the selected axis
   */
  public abstract double getRawAxis(int which);

  /**
   * Is the trigger pressed
   *$
   * @return true if pressed
   */
  public final boolean getTrigger() {
    return getTrigger(Hand.kRight);
  }

  /**
   * Is the trigger pressed
   *$
   * @param hand which hand
   * @return true if the trigger for the given hand is pressed
   */
  public abstract boolean getTrigger(Hand hand);

  /**
   * Is the top button pressed
   *$
   * @return true if the top button is pressed
   */
  public final boolean getTop() {
    return getTop(Hand.kRight);
  }

  /**
   * Is the top button pressed
   *$
   * @param hand which hand
   * @return true if hte top button for the given hand is pressed
   */
  public abstract boolean getTop(Hand hand);

  /**
   * Is the bumper pressed
   *$
   * @return true if the bumper is pressed
   */
  public final boolean getBumper() {
    return getBumper(Hand.kRight);
  }

  /**
   * Is the bumper pressed
   *$
   * @param hand which hand
   * @return true if hte bumper is pressed
   */
  public abstract boolean getBumper(Hand hand);

  /**
   * Is the given button pressed
   *$
   * @param button which button number
   * @return true if the button is pressed
   */
  public abstract boolean getRawButton(int button);

  public abstract int getPOV(int pov);

  public int getPOV() {
    return getPOV(0);
  }
}
