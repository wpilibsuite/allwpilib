// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

public class SerialPortJNI extends JNIWrapper {
  public static native int serialInitializePort(byte port);

  public static native int serialInitializePortDirect(byte port, String portName);

  public static native void serialSetBaudRate(int handle, int baud);

  public static native void serialSetDataBits(int handle, byte bits);

  public static native void serialSetParity(int handle, byte parity);

  public static native void serialSetStopBits(int handle, byte stopBits);

  public static native void serialSetWriteMode(int handle, byte mode);

  public static native void serialSetFlowControl(int handle, byte flow);

  public static native void serialSetTimeout(int handle, double timeout);

  public static native void serialEnableTermination(int handle, char terminator);

  public static native void serialDisableTermination(int handle);

  public static native void serialSetReadBufferSize(int handle, int size);

  public static native void serialSetWriteBufferSize(int handle, int size);

  public static native int serialGetBytesReceived(int handle);

  public static native int serialRead(int handle, byte[] buffer, int count);

  public static native int serialWrite(int handle, byte[] buffer, int count);

  public static native void serialFlush(int handle);

  public static native void serialClear(int handle);

  public static native void serialClose(int handle);
}
