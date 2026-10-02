// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.driverstation;

/** Represents a finger on a touchpad. */
public final class TouchpadFinger {
  /** Whether the finger is touching the touchpad. */
  public final boolean down;

  /** The x position of the finger. 0 is at top left. */
  public final float x;

  /** The y position of the finger. 0 is at top left. */
  public final float y;

  /**
   * Creates a TouchpadFinger object.
   *
   * @param down Whether the finger is touching the touchpad.
   * @param x The x position of the finger.
   * @param y The y position of the finger.
   */
  public TouchpadFinger(boolean down, float x, float y) {
    this.x = x;
    this.y = y;
    this.down = down;
  }
}
