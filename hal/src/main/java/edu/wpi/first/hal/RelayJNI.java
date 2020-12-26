// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

public class RelayJNI extends DIOJNI {
  public static native int initializeRelayPort(int halPortHandle, boolean forward);

  public static native void freeRelayPort(int relayPortHandle);

  public static native boolean checkRelayChannel(int channel);

  public static native void setRelay(int relayPortHandle, boolean on);

  public static native boolean getRelay(int relayPortHandle);
}
