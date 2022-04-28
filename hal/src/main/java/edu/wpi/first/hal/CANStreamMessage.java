// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

public class CANStreamMessage {
  @SuppressWarnings("MemberName")
  public final byte[] data = new byte[8];

  @SuppressWarnings("MemberName")
  public int length;

  @SuppressWarnings("MemberName")
  public long timestamp;

  @SuppressWarnings("MemberName")
  public int messageID;

  /**
   * API used from JNI to set the data.
   *
   * @param messageID CAN message ID of the message.
   * @param length Length of packet in bytes.
   * @param timestamp CAN frame timestamp in microseconds.
   * @return Buffer containing CAN frame.
   */
  @SuppressWarnings("PMD.MethodReturnsInternalArray")
  public byte[] setStreamData(int messageID, int length, long timestamp) {
    this.messageID = messageID;
    this.length = length;
    this.timestamp = timestamp;
    return data;
  }
}
