/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

/**
 * Gamepad Interface.
 */
public abstract class GamepadBase extends GenericHID {
  public GamepadBase(int port) {
    super(port);
  }

  public abstract double getRawAxis(int axis);

  /**
   * Is the bumper pressed.
   *
   * @param hand which hand
   * @return true if the bumper is pressed
   */
  public abstract boolean getBumper(Hand hand);

  /**
   * Is the bumper pressed.
   *
   * @return true if the bumper is pressed
   */
  public boolean getBumper() {
    return getBumper(Hand.kRight);
  }

  public abstract boolean getStickButton(Hand hand);

  public boolean getStickButton() {
    return getStickButton(Hand.kRight);
  }

  public abstract boolean getRawButton(int button);

  public abstract int getPOV(int pov);

  public abstract int getPOVCount();

  public abstract HIDType getType();

  public abstract String getName();

  public abstract void setOutput(int outputNumber, boolean value);

  public abstract void setOutputs(int value);

  public abstract void setRumble(RumbleType type, double value);
}
