// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

import java.nio.ByteBuffer;

public class SPIJNI extends JNIWrapper {
  public static final int INVALID_PORT = -1;
  public static final int ONBOARD_CS0_PORT = 0;
  public static final int ONBOARD_CS1_PORT = 1;
  public static final int ONBOARD_CS2_PORT = 2;
  public static final int ONBOARD_CS3_PORT = 3;
  public static final int MXP_PORT = 4;

  public static final int SPI_MODE0 = 0;
  public static final int SPI_MODE1 = 1;
  public static final int SPI_MODE2 = 2;
  public static final int SPI_MODE3 = 3;

  public static native void spiInitialize(int port);

  public static native int spiTransaction(
      int port, ByteBuffer dataToSend, ByteBuffer dataReceived, byte size);

  public static native int spiTransactionB(
      int port, byte[] dataToSend, byte[] dataReceived, byte size);

  public static native int spiWrite(int port, ByteBuffer dataToSend, byte sendSize);

  public static native int spiWriteB(int port, byte[] dataToSend, byte sendSize);

  public static native int spiRead(int port, boolean initiate, ByteBuffer dataReceived, byte size);

  public static native int spiReadB(int port, boolean initiate, byte[] dataReceived, byte size);

  public static native void spiClose(int port);

  public static native void spiSetSpeed(int port, int speed);

  public static native void spiSetMode(int port, int mode);

  public static native int spiGetMode(int port);

  public static native void spiSetChipSelectActiveHigh(int port);

  public static native void spiSetChipSelectActiveLow(int port);

  public static native void spiInitAuto(int port, int bufferSize);

  public static native void spiFreeAuto(int port);

  public static native void spiStartAutoRate(int port, double period);

  public static native void spiStartAutoTrigger(
      int port,
      int digitalSourceHandle,
      int analogTriggerType,
      boolean triggerRising,
      boolean triggerFalling);

  public static native void spiStopAuto(int port);

  public static native void spiSetAutoTransmitData(int port, byte[] dataToSend, int zeroSize);

  public static native void spiForceAutoRead(int port);

  public static native int spiReadAutoReceivedData(
      int port, ByteBuffer buffer, int numToRead, double timeout);

  public static native int spiReadAutoReceivedData(
      int port, int[] buffer, int numToRead, double timeout);

  public static native int spiGetAutoDroppedCount(int port);

  public static native void spiConfigureAutoStall(
      int port, int csToSclkTicks, int stallTicks, int pow2BytesPerRead);
}
