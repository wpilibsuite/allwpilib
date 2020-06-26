/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.hal;

import java.nio.ByteBuffer;

@SuppressWarnings("AbbreviationAsWordInName")
public class I2CJNI extends JNIWrapper {
  public static class NativeI2CBus implements I2CBus {
    public NativeI2CBus(long impl) {
      m_impl = impl;
    }

    public boolean openBus() {
      return nativeOpenBus(m_impl);
    }

    public void closeBus() {
      nativeCloseBus(m_impl);
    }

    public boolean initialize(int deviceAddress) {
      return nativeInitialize(m_impl, deviceAddress);
    }

    public int transaction(int deviceAddress, byte[] dataToSend, int sendSize, byte[] dataReceived,
                           int receiveSize) {
      return nativeTransaction(m_impl, deviceAddress, dataToSend, sendSize, dataReceived,
                               receiveSize);
    }

    public int write(int deviceAddress, byte[] dataToSend, int sendSize) {
      return nativeWrite(m_impl, deviceAddress, dataToSend, sendSize);
    }

    public int read(int deviceAddress, byte[] buffer, int count) {
      return nativeRead(m_impl, deviceAddress, buffer, count);
    }

    private long m_impl;
  }

  public static native boolean registerI2CBus(int bus, I2CBus impl);
  public static native I2CBus getSystemI2CBus(int bus);
  public static native I2CBus getRegisteredI2CBus(int bus);
  public static native I2CBus unregisterI2CBus(int bus);

  private static native boolean nativeOpenBus(long impl);
  private static native void nativeCloseBus(long impl);
  private static native boolean nativeInitialize(long impl, int deviceAddress);
  private static native int nativeTransaction(long impl, int deviceAddress,
                                              byte[] dataToSend, int sendSize, byte[] dataReceived,
                                              int receiveSize);
  private static native int nativeWrite(long impl, int deviceAddress, byte[] dataToSend,
                                        int sendSize);
  private static native int nativeRead(long impl, int deviceAddress, byte[] buffer,
                                       int count);

  public static native int i2CInitialize(int bus, int address);

  public static native int i2CTransaction(int handle, ByteBuffer dataToSend, byte sendSize,
                                          ByteBuffer dataReceived, byte receiveSize);

  public static native int i2CTransactionB(int handle, byte[] dataToSend, byte sendSize,
                                           byte[] dataReceived, byte receiveSize);

  public static native int i2CWrite(int handle, ByteBuffer dataToSend, byte sendSize);

  public static native int i2CWriteB(int handle, byte[] dataToSend, byte sendSize);

  public static native int i2CRead(int handle, ByteBuffer dataReceived, byte receiveSize);

  public static native int i2CReadB(int handle, byte[] dataReceived, byte receiveSize);

  public static native void i2CClose(int handle);
}
