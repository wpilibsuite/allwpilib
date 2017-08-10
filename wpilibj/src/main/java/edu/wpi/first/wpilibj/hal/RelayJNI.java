/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.hal;

public class RelayJNI extends DIOJNI {
  public static native int initializeRelayPort(int halPortHandle, boolean forward);

  public static native void freeRelayPort(int relayPortHandle);

  public static native boolean checkRelayChannel(int channel);

  public static native void setRelay(int relayPortHandle, boolean on);

  public static native boolean getRelay(int relayPortHandle);
}
