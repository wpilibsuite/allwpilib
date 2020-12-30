// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

public class CANData {
  @SuppressWarnings("MemberName")
  public final byte[] data = new byte[8];

  @SuppressWarnings("MemberName")
  public int length;

  @SuppressWarnings("MemberName")
  public long timestamp;

  /** API used from JNI to set the data. */
  @SuppressWarnings("PMD.MethodReturnsInternalArray")
  public byte[] setData(int length, long timestamp) {
    this.length = length;
    this.timestamp = timestamp;
    return data;
  }
}
