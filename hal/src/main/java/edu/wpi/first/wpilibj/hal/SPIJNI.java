/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.hal;

import java.nio.ByteBuffer;

@SuppressWarnings("AbbreviationAsWordInName")
public class SPIJNI extends JNIWrapper {
  public static native void spiInitialize(int port);

  public static native int spiTransaction(int port, ByteBuffer dataToSend,
                                          ByteBuffer dataReceived, byte size);

  public static native int spiTransactionB(int port, byte[] dataToSend,
                                           byte[] dataReceived, byte size);

  public static native int spiWrite(int port, ByteBuffer dataToSend, byte sendSize);

  public static native int spiWriteB(int port, byte[] dataToSend, byte sendSize);

  public static native int spiRead(int port, boolean initiate, ByteBuffer dataReceived, byte size);

  public static native int spiReadB(int port, boolean initiate, byte[] dataReceived, byte size);

  public static native void spiClose(int port);

  public static native void spiSetSpeed(int port, int speed);

  public static native void spiSetOpts(int port, int msbFirst, int sampleOnTrailing,
                                       int clkIdleHigh);

  public static native void spiSetChipSelectActiveHigh(int port);

  public static native void spiSetChipSelectActiveLow(int port);

  public static native void spiInitAuto(int port, int bufferSize);

  public static native void spiFreeAuto(int port);

  public static native void spiStartAutoRate(int port, double period);

  public static native void spiStartAutoTrigger(int port, int digitalSourceHandle,
                                                int analogTriggerType, boolean triggerRising,
                                                boolean triggerFalling);

  public static native void spiStopAuto(int port);

  public static native void spiSetAutoTransmitData(int port, byte[] dataToSend, int zeroSize);

  public static native void spiForceAutoRead(int port);

  public static native int spiReadAutoReceivedData(int port, ByteBuffer buffer, int numToRead,
                                                   double timeout);

  public static native int spiReadAutoReceivedData(int port, byte[] buffer, int numToRead,
                                                   double timeout);

  public static native int spiGetAutoDroppedCount(int port);
}
