// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal.can;

import edu.wpi.first.hal.CANStreamMessage;
import java.io.IOException;

/**
 * Exception indicating that a CAN stream overflowed at some point between reads, therefore some
 * messages were lost. This exception contains any messages that were successfully read during the
 * operation that threw the exception.
 */
public class CANStreamOverflowException extends IOException {
  /** The messages. */
  private final CANStreamMessage[] m_messages;

  /** The length of messages read. */
  private final int m_messagesRead;

  /**
   * Constructs a new CANStreamOverflowException.
   *
   * @param messages The messages that were read successfully.
   * @param messagesRead The length of messages read successfully.
   */
  @SuppressWarnings("PMD.ArrayIsStoredDirectly")
  public CANStreamOverflowException(CANStreamMessage[] messages, int messagesRead) {
    super("A CAN Stream has overflowed. Data will be missed");
    this.m_messages = messages;
    this.m_messagesRead = messagesRead;
  }

  /**
   * Gets the messages that were successfully read. Use {@link #getMessagesRead()} to determine how
   * many messages in the returned array are valid.
   *
   * @return the messages
   */
  @SuppressWarnings("PMD.MethodReturnsInternalArray")
  public CANStreamMessage[] getMessages() {
    return m_messages;
  }

  /**
   * Gets the count of messages that were successfully read.
   *
   * @return count of messages
   */
  public int getMessagesRead() {
    return m_messagesRead;
  }
}
