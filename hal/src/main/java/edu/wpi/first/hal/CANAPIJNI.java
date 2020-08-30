/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.hal;

@SuppressWarnings("AbbreviationAsWordInName")
public class CANAPIJNI extends JNIWrapper {
  public static native int initializeCAN(int manufacturer, int deviceId, int deviceType);

  public static native void cleanCAN(int handle);

  public static native void writeCANPacket(int handle, byte[] data, int apiId);

  public static native void writeCANPacketRepeating(int handle, byte[] data, int apiId,
                                                    int repeatMs);

  public static native void writeCANRTRFrame(int handle, int length, int apiId);

  public static native int writeCANPacketNoThrow(int handle, byte[] data, int apiId);

  public static native int writeCANPacketRepeatingNoThrow(int handle, byte[] data, int apiId,
                                                         int repeatMs);

  public static native int writeCANRTRFrameNoThrow(int handle, int length, int apiId);

  public static native void stopCANPacketRepeating(int handle, int apiId);

  public static native boolean readCANPacketNew(int handle, int apiId, CANData data);

  public static native boolean readCANPacketLatest(int handle, int apiId, CANData data);

  public static native boolean readCANPacketTimeout(int handle, int apiId, int timeoutMs,
                                                 CANData data);
}
