package edu.wpi.first.wpilibj.hal;

import java.nio.ByteBuffer;

public class I2CJNI extends JNIWrapper {
  public static native void i2CInitialize(byte port);

  public static native int i2CTransaction(byte port, byte address, ByteBuffer dataToSend,
      byte sendSize, ByteBuffer dataReceived, byte receiveSize);

  public static native int i2CWrite(byte port, byte address, ByteBuffer dataToSend, byte sendSize);

  public static native int i2CRead(byte port, byte address, ByteBuffer dataRecieved,
      byte receiveSize);

  public static native void i2CClose(byte port);
}
