/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.hal;

import java.nio.ByteBuffer;

public class SerialPortJNI extends JNIWrapper {
  public static native void serialInitializePort(byte port);

  public static native void serialSetBaudRate(byte port, int baud);

  public static native void serialSetDataBits(byte port, byte bits);

  public static native void serialSetParity(byte port, byte parity);

  public static native void serialSetStopBits(byte port, byte stopBits);

  public static native void serialSetWriteMode(byte port, byte mode);

  public static native void serialSetFlowControl(byte port, byte flow);

  public static native void serialSetTimeout(byte port, float timeout);

  public static native void serialEnableTermination(byte port, char terminator);

  public static native void serialDisableTermination(byte port);

  public static native void serialSetReadBufferSize(byte port, int size);

  public static native void serialSetWriteBufferSize(byte port, int size);

  public static native int serialGetBytesRecieved(byte port);

  public static native int serialRead(byte port, ByteBuffer buffer, int count);

  public static native int serialWrite(byte port, ByteBuffer buffer, int count);

  public static native void serialFlush(byte port);

  public static native void serialClear(byte port);

  public static native void serialClose(byte port);
}
