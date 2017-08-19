/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.hal;

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

  public static native int getNumPCMModules();

  public static native int getNumSolenoidChannels();

  public static native int getNumPDPModules();

  public static native int getNumPDPChannels();
}
