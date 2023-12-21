// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

public class LEDJNI extends JNIWrapper {
  /**
   * Set the state of the "Radio" LED.
   *
   * @param state The state to set the LED to.
   * @see "HAL_SetRadioLEDState"
   */
  public static native void setRadioLEDState(int state);
}
