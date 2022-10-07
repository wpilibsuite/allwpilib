// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

public class PortsJNI extends JNIWrapper {
  public static native int getNumAccumulators();

  public static native int getNumAnalogTriggers();

  public static native int getNumAnalogInputs();

  public static native int getNumAnalogOutputs();

  public static native int getNumCounters();

  public static native int getNumDigitalHeaders();

  public static native int getNumPWMHeaders();

  public static native int getNumDigitalChannels();

  public static native int getNumPWMChannels();

  public static native int getNumDigitalPWMOutputs();

  public static native int getNumEncoders();

  public static native int getNumInterrupts();

  public static native int getNumRelayChannels();

  public static native int getNumRelayHeaders();

  public static native int getNumCTREPCMModules();

  public static native int getNumCTRESolenoidChannels();

  public static native int getNumCTREPDPModules();

  public static native int getNumCTREPDPChannels();

  public static native int getNumREVPDHModules();

  public static native int getNumREVPDHChannels();

  public static native int getNumREVPHModules();

  public static native int getNumREVPHChannels();
}
