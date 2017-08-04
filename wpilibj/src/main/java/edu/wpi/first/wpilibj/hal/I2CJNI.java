/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.hal;

import java.nio.ByteBuffer;

@SuppressWarnings("AbbreviationAsWordInName")
public class I2CJNI extends JNIWrapper {
  public static native void i2CInitialize(int port);

  public static native int i2CTransaction(int port, byte address, ByteBuffer dataToSend,
                                          byte sendSize, ByteBuffer dataReceived, byte receiveSize);

  public static native int i2CWrite(int port, byte address, ByteBuffer dataToSend, byte sendSize);

  public static native int i2CRead(int port, byte address, ByteBuffer dataRecieved,
                                   byte receiveSize);

  public static native void i2CClose(int port);
}
