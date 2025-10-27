// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal.can;

/** Represents a CAN message read from a stream. */
public class CANReceiveMessage {
  /** The message data. */
  @SuppressWarnings("MemberName")
  public final byte[] data = new byte[64];

  /** The length of the data received (0-8 bytes). */
  @SuppressWarnings("MemberName")
  public int length;

  /** The flags of the message. */
  @SuppressWarnings("MemberName")
  public int flags;

  /** Timestamp message was received, in microseconds (wpi time). */
  @SuppressWarnings("MemberName")
  public long timestamp;

  /** Default constructor. */
  public CANReceiveMessage() {}

  /**
   * API used from JNI to set the data.
   *
   * @param length Length of packet in bytes.
   * @param flags Message flags.
   * @param timestamp CAN frame timestamp in microseconds.
   * @return Buffer containing CAN frame.
   */
  @SuppressWarnings("PMD.MethodReturnsInternalArray")
  public byte[] setReceiveData(int length, int flags, long timestamp) {
    this.flags = flags;
    this.length = length;
    this.timestamp = timestamp;
    return data;
  }
}
