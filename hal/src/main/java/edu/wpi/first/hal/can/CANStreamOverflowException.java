// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal.can;

import edu.wpi.first.hal.CANStreamMessage;
import java.io.IOException;

public class CANStreamOverflowException extends IOException {
  private final CANStreamMessage[] m_messages;
  private final int m_messagesRead;

  /**
   * Constructs a new CANStreamOverflowException.
   * 
   * @param messages The messages
   * @param messagesRead The length of messages read
   */
  public CANStreamOverflowException(CANStreamMessage[] messages, int messagesRead) {
    super("A CAN Stream has overflowed. Data will be missed");
    this.m_messages = messages;
    this.m_messagesRead = messagesRead;
  }

  public CANStreamMessage[] getMessages() {
    return m_messages;
  }

  public int getMessagesRead() {
    return m_messagesRead;
  }
}
