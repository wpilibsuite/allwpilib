// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal.simulation;

import edu.wpi.first.hal.JNIWrapper;

public class NotifierDataJNI extends JNIWrapper {
  public static native long getNextTimeout();

  public static native int getNumNotifiers();
}
