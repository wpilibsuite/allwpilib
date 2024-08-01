// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

/** Represents a CAN message read from a stream. */
public class CANStreamMessage {
  /** The message data. */
  @SuppressWarnings("MemberName")
  public final byte[] data = new byte[8];

  /** The length of the data received (0-8 bytes). */
  @SuppressWarnings("MemberName")
  public int length;

  /** Timestamp message was received, in milliseconds (based off of CLOCK_MONOTONIC). */
  @SuppressWarnings("MemberName")
  public long timestamp;

  /** The message ID. */
  @SuppressWarnings("MemberName")
  public int messageID;

  /** Default constructor. */
  public CANStreamMessage() {}

  /**
   * API used from JNI to set the data.
   *
   * @param length Length of packet in bytes.
   * @param messageID CAN message ID of the message.
   * @param timestamp CAN frame timestamp in microseconds.
   * @return Buffer containing CAN frame.
   */
  @SuppressWarnings("PMD.MethodReturnsInternalArray")
  public byte[] setStreamData(int length, int messageID, long timestamp) {
    this.messageID = messageID;
    this.length = length;
    this.timestamp = timestamp;
    return data;
  }
}
