// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

import java.nio.ByteBuffer;

public class I2CJNI extends JNIWrapper {
  public static native void i2CInitialize(int port);

  public static native int i2CTransaction(
      int port,
      byte address,
      ByteBuffer dataToSend,
      byte sendSize,
      ByteBuffer dataReceived,
      byte receiveSize);

  public static native int i2CTransactionB(
      int port,
      byte address,
      byte[] dataToSend,
      byte sendSize,
      byte[] dataReceived,
      byte receiveSize);

  public static native int i2CWrite(int port, byte address, ByteBuffer dataToSend, byte sendSize);

  public static native int i2CWriteB(int port, byte address, byte[] dataToSend, byte sendSize);

  public static native int i2CRead(
      int port, byte address, ByteBuffer dataReceived, byte receiveSize);

  public static native int i2CReadB(int port, byte address, byte[] dataReceived, byte receiveSize);

  public static native void i2CClose(int port);
}
