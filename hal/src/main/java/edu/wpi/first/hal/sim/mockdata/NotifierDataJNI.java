/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.hal.sim.mockdata;

import edu.wpi.first.hal.JNIWrapper;

public class NotifierDataJNI extends JNIWrapper {
  public static native long getNextTimeout();
  public static native int getNumNotifiers();
}
