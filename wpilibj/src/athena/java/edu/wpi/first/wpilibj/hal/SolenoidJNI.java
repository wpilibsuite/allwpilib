/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.hal;

public class SolenoidJNI extends JNIWrapper {
  public static native long initializeSolenoidPort(long portPointer);

  public static native void freeSolenoidPort(long port_pointer);

  public static native long getPortWithModule(byte module, byte channel);

  public static native void setSolenoid(long port, boolean on);

  public static native boolean getSolenoid(long port);

  public static native byte getAllSolenoids(long port);

  public static native int getPCMSolenoidBlackList(long pcm_pointer);

  public static native boolean getPCMSolenoidVoltageStickyFault(long pcm_pointer);

  public static native boolean getPCMSolenoidVoltageFault(long pcm_pointer);

  public static native void clearAllPCMStickyFaults(long pcm_pointer);
}
