/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.hal;

import java.nio.ByteBuffer;
import java.nio.IntBuffer;
import java.nio.LongBuffer;

public class SPIJNI extends JNIWrapper {
  public static native void spiInitialize(byte port);

  public static native int spiTransaction(byte port, ByteBuffer dataToSend,
      ByteBuffer dataReceived, byte size);

  public static native int spiWrite(byte port, ByteBuffer dataToSend, byte sendSize);

  public static native int spiRead(byte port, ByteBuffer dataReceived, byte size);

  public static native void spiClose(byte port);

  public static native void spiSetSpeed(byte port, int speed);

  public static native void spiSetOpts(byte port, int msb_first, int sample_on_trailing,
      int clk_idle_high);

  public static native void spiSetChipSelectActiveHigh(byte port);

  public static native void spiSetChipSelectActiveLow(byte port);

  public static native void spiInitAccumulator(byte port, int period, int cmd,
      byte xferSize, int validMask, int validValue, byte dataShift,
      byte dataSize, boolean isSigned, boolean bigEndian);

  public static native void spiFreeAccumulator(byte port);

  public static native void spiResetAccumulator(byte port);

  public static native void spiSetAccumulatorCenter(byte port, int center);

  public static native void spiSetAccumulatorDeadband(byte port, int deadband);

  public static native int spiGetAccumulatorLastValue(byte port);

  public static native long spiGetAccumulatorValue(byte port);

  public static native int spiGetAccumulatorCount(byte port);

  public static native double spiGetAccumulatorAverage(byte port);

  public static native void spiGetAccumulatorOutput(byte port, LongBuffer value,
      IntBuffer count);
}
