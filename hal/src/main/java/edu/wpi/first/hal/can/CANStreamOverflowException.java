package edu.wpi.first.hal.can;

import java.io.IOException;

import edu.wpi.first.hal.CANStreamMessage;

public class CANStreamOverflowException extends IOException {
  private final CANStreamMessage[] messages;
  private final int length;

  public CANStreamOverflowException(CANStreamMessage[] messages, int length) {
    super("A CAN Stream has overflowed. Data will be missed");
    this.messages = messages;
    this.length = length;
  }

  public CANStreamMessage[] getMessages() {
    return messages;
  }

  public int getLength() {
    return length;
  }
}
