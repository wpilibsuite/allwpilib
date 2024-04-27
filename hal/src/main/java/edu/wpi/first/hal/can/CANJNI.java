// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal.can;

import edu.wpi.first.hal.CANStreamMessage;
import edu.wpi.first.hal.JNIWrapper;
import java.nio.ByteBuffer;
import java.nio.IntBuffer;

/**
 * CAN API HAL JNI Functions.
 *
 * @see "hal/CAN.h"
 */
@SuppressWarnings("MethodName")
public class CANJNI extends JNIWrapper {
  /** Flag for sending a CAN message once. */
  public static final int CAN_SEND_PERIOD_NO_REPEAT = 0;

  /** Flag for stopping periodic CAN message sends. */
  public static final int CAN_SEND_PERIOD_STOP_REPEATING = -1;

  /** Mask for "is frame remote" in message ID. */
  public static final int CAN_IS_FRAME_REMOTE = 0x80000000;

  /** Mask for "is frame 11 bits" in message ID. */
  public static final int CAN_IS_FRAME_11BIT = 0x40000000;

  /** Default constructor. */
  public CANJNI() {}

  /**
   * Sends a CAN message.
   *
   * @param messageID The ID of the CAN message.
   * @param data The data bytes to be sent.
   * @param periodMs The period in milliseconds at which to send the message, use {@link
   *     #CAN_SEND_PERIOD_NO_REPEAT} for a single send.
   */
  public static native void FRCNetCommCANSessionMuxSendMessage(
      int messageID, byte[] data, int periodMs);

  /**
   * Receives a CAN message.
   *
   * @param messageID store for the received message ID (output parameter).
   * @param messageIDMask the message ID mask to look for
   * @param timeStamp the packet received timestamp (based off of CLOCK_MONOTONIC) (output
   *     parameter).
   * @return The data bytes of the received message.
   */
  public static native byte[] FRCNetCommCANSessionMuxReceiveMessage(
      IntBuffer messageID, int messageIDMask, ByteBuffer timeStamp);

  /**
   * Retrieves the current status of the CAN bus.
   *
   * @param status The CANStatus object to hold the retrieved status.
   */
  public static native void getCANStatus(CANStatus status);

  /**
   * Opens a new CAN stream session for receiving CAN messages with specified filters.
   *
   * @param messageID The CAN messageID to match against. The bits of the messageID are bitwise
   *     ANDed with the messageIDMask.
   * @param messageIDMask The CAN messageIDMask is a bit-wise mask of bits in the messageID to match
   *     against. This allows matching against multiple frames. For example, providing an messageID
   *     of 0x2050001 and a mask of 0x1FFF003F would match all REV motor controller frames for a
   *     device with CAN ID 1. Providing a mask of 0x1FFFFFFF means that only the exact messageID
   *     will be matched. Providing a mask of 0 would match any frame of any type.
   * @param maxMessages The maximum number of messages that can be buffered in the session.
   * @return The handle to the opened CAN stream session.
   */
  public static native int openCANStreamSession(int messageID, int messageIDMask, int maxMessages);

  /**
   * Closes a CAN stream session.
   *
   * @param sessionHandle The handle of the CAN stream session to be closed.
   */
  public static native void closeCANStreamSession(int sessionHandle);

  /**
   * Reads messages from a CAN stream session.
   *
   * @param sessionHandle The handle of the CAN stream session.
   * @param messages An array to hold the CANStreamMessage objects (output parameter).
   * @param messagesToRead The number of messages to read from the session.
   * @return The number of messages read into the buffer
   * @throws CANStreamOverflowException If the number of messages to read exceeds the capacity of
   *     the provided messages array.
   */
  public static native int readCANStreamSession(
      int sessionHandle, CANStreamMessage[] messages, int messagesToRead)
      throws CANStreamOverflowException;
}
