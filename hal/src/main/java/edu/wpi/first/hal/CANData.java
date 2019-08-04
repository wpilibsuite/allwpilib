/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.hal;

public class CANData {
  @SuppressWarnings("MemberName")
  public final byte[] data = new byte[8];
  @SuppressWarnings("MemberName")
  public int length;
  @SuppressWarnings("MemberName")
  public long timestamp;

  /**
   * API used from JNI to set the data.
   */
  @SuppressWarnings("PMD.MethodReturnsInternalArray")
  public byte[] setData(int length, long timestamp) {
    this.length = length;
    this.timestamp = timestamp;
    return data;
  }
}
