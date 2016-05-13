/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.hal;

public class RelayJNI extends DIOJNI {
  public static native void setRelayForward(long digitalPortPointer, boolean on);

  public static native void setRelayReverse(long digitalPortPointer, boolean on);

  public static native boolean getRelayForward(long digitalPortPointer);

  public static native boolean getRelayReverse(long digitalPortPointer);
}
