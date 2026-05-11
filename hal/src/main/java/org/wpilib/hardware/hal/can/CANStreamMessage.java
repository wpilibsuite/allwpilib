// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.hal.can;

/** Represents a CAN message read from a stream. */
public class CANStreamMessage {
  /** The message data. */
  public final byte[] data = new byte[64];

  /** The length of the data received (0-64 bytes). */
  public int length;

  /** The flags of the message. */
  public int flags;

  /** Timestamp message was received, in milliseconds (based off of CLOCK_MONOTONIC). */
  public long timestamp;

  /** The message ID. */
  public int messageId;

  /** Default constructor. */
  public CANStreamMessage() {}

  /**
   * API used from JNI to set the data.
   *
   * @param length Length of packet in bytes.
   * @param messageId CAN message ID of the message.
   * @param flags Message flags.
   * @param timestamp CAN frame timestamp in microseconds.
   * @return Buffer containing CAN frame.
   */
  @SuppressWarnings("PMD.MethodReturnsInternalArray")
  public byte[] setStreamData(int length, int flags, int messageId, long timestamp) {
    this.messageId = messageId;
    this.flags = flags;
    this.length = length;
    this.timestamp = timestamp;
    return data;
  }
}
