/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.hal.simulation;

import edu.wpi.first.hal.I2CBus;
import edu.wpi.first.hal.JNIWrapper;

public class I2CDataJNI extends JNIWrapper {
  public static class NativeI2CDevice implements I2CDevice {
    public NativeI2CDevice(long impl) {
      m_impl = impl;
    }

    public boolean initialize() {
      return nativeDeviceInitialize(m_impl);
    }

    public int transaction(byte[] dataToSend, int sendSize, byte[] dataReceived,
                           int receiveSize) {
      return nativeDeviceTransaction(m_impl, dataToSend, sendSize, dataReceived,
                                     receiveSize);
    }

    public int write(byte[] dataToSend, int sendSize) {
      return nativeDeviceWrite(m_impl, dataToSend, sendSize);
    }

    public int read(byte[] buffer, int count) {
      return nativeDeviceRead(m_impl, buffer, count);
    }

    private long m_impl;
  }

  public static native I2CBus getSimBus();
  public static native boolean registerDevice(int bus, int deviceAddress, I2CDevice device);
  public static native I2CDevice unregisterDevice(int bus, int deviceAddress);

  private static native boolean nativeDeviceInitialize(long impl);
  private static native int nativeDeviceTransaction(long impl, byte[] dataToSend, int sendSize,
                                                    byte[] dataReceived, int receiveSize);
  private static native int nativeDeviceWrite(long impl, byte[] dataToSend, int sendSize);
  private static native int nativeDeviceRead(long impl, byte[] buffer, int count);
}
