/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

/**
 * Gamepad Interface.
 *
 * @deprecated Inherit directly from GenericHID instead.
 */
@Deprecated
public abstract class GamepadBase extends GenericHID {
  public GamepadBase(int port) {
    super(port);
  }

  @Override
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

  @Override
  public abstract boolean getRawButton(int button);

  @Override
  public abstract int getPOV(int pov);

  @Override
  public abstract int getPOVCount();

  @Override
  public abstract HIDType getType();

  @Override
  public abstract String getName();

  @Override
  public abstract void setOutput(int outputNumber, boolean value);

  @Override
  public abstract void setOutputs(int value);

  @Override
  public abstract void setRumble(RumbleType type, double value);
}
