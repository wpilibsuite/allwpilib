// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

@SuppressWarnings("MemberName")
public class CANData {
  public final byte[] data = new byte[8];
  public int length;
  public long timestamp;

  /**
   * API used from JNI to set the data.
   *
   * @param length Length of packet in bytes.
   * @param timestamp CAN frame timestamp in microseconds.
   * @return Buffer containing CAN frame.
   */
  @SuppressWarnings("PMD.MethodReturnsInternalArray")
  public byte[] setData(int length, long timestamp) {
    this.length = length;
    this.timestamp = timestamp;
    return data;
  }
}
