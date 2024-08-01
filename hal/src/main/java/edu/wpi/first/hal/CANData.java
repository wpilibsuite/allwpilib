// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

/** Represents a received CAN message. */
@SuppressWarnings("MemberName")
public class CANData {
  /** Contents of the CAN frame. */
  public final byte[] data = new byte[8];

  /** Length of the frame in bytes. */
  public int length;

  /** CAN frame timestamp in milliseconds. */
  public long timestamp;

  /** Default constructor. */
  public CANData() {}

  /**
   * API used from JNI to set the data.
   *
   * @param length Length of packet in bytes.
   * @param timestamp CAN frame timestamp in milliseconds.
   * @return Buffer to place CAN frame data in.
   */
  @SuppressWarnings("PMD.MethodReturnsInternalArray")
  public byte[] setData(int length, long timestamp) {
    this.length = length;
    this.timestamp = timestamp;
    return data;
  }
}
