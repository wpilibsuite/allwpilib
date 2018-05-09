/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.hal;

import edu.wpi.first.wpilibj.CANData;

@SuppressWarnings("AbbreviationAsWordInName")
public class CANAPIJNI extends JNIWrapper {
  public static native int initializeCAN(int manufacturer, int deviceId, int deviceType);

  public static native void cleanCAN(int handle);

  public static native void writeCANPacket(int handle, byte[] data, int apiId);

  public static native void writeCANPacketRepeating(int handle, byte[] data, int apiId,
                                                    int repeatMs);

  public static native void stopCANPacketRepeating(int handle, int apiId);

  public static native void readCANPacketNew(int handle, int apiId, CANData data);

  public static native void readCANPacketLatest(int handle, int apiId, CANData data);

  public static native void readCANPacketTimeout(int handle, int apiId, int timeoutMs,
                                                 CANData data);

  public static native void readCANPeriodicPacket(int handle, int apiId, int timeoutMs,
                                                  int periodMs, CANData data);
}
