/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.hal;

import edu.wpi.first.wpilibj.AccumulatorResult;
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

  public static native void spiInitAccumulator(int port, int period, int cmd, byte xferSize,
                                               int validMask, int validValue, byte dataShift,
                                               byte dataSize, boolean isSigned, boolean bigEndian);

  public static native void spiFreeAccumulator(int port);

  public static native void spiResetAccumulator(int port);

  public static native void spiSetAccumulatorCenter(int port, int center);

  public static native void spiSetAccumulatorDeadband(int port, int deadband);

  public static native int spiGetAccumulatorLastValue(int port);

  public static native long spiGetAccumulatorValue(int port);

  public static native int spiGetAccumulatorCount(int port);

  public static native double spiGetAccumulatorAverage(int port);

  public static native void spiGetAccumulatorOutput(int port, AccumulatorResult result);
}
