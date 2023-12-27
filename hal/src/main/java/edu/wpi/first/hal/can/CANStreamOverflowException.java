// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal.can;

import edu.wpi.first.hal.CANStreamMessage;
import java.io.IOException;

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
